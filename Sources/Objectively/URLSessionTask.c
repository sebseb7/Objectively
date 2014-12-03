/*
 * Objectively: Ultra-lightweight object oriented framework for c99.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <assert.h>
#include <stdlib.h>
#include <curl/curl.h>

#include <Objectively/URLRequest.h>
#include <Objectively/URLSession.h>
#include <Objectively/URLSessionTask.h>

#define __Class __URLSessionTask

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	return NULL;
}

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	URLSessionTask *this = (URLSessionTask *) self;

	if (this->error) {
		free(this->error);
	}

	release(this->locals.thread);
	release(this->request);
	release(this->session);

	super(Object, self, dealloc);
}

#pragma mark - URLSessionTaskInterface

/**
 * @see URLSessionTaskInterface::cancel(URLSessionTask *)
 */
static void cancel(URLSessionTask *self) {

	if (self->state == TASK_RUNNING || self->state == TASK_SUSPENDED) {

		// TODO
		self->state = TASK_CANCELING;
	}
}

/**
 * @brief Thread function which coordinates this task.
 */
static id run(Thread *thread) {

	URLSessionTask *self = thread->data;

	$(self, setup);

	assert(self->locals.handle);
	const CURLcode result = curl_easy_perform(self->locals.handle);

	$(self, teardown);

	if (self->completion) {
		self->completion(self, result == CURLE_OK);
	}

	return NULL;
}

/**
 * @see URLSessionTaskInterface::initWithRequestInSession(URLSessionTask *, URLRequest *, URLSession *, URLSessionTaskCompletion)
 */
static URLSessionTask *initWithRequestInSession(URLSessionTask *self, struct URLRequest *request,
		struct URLSession *session, URLSessionTaskCompletion completion) {

	assert(request);
	assert(session);

	self = (URLSessionTask *) super(Object, self, init);
	if (self) {
		self->locals.thread = $(alloc(Thread), initWithFunction, run, self);

		self->error = calloc(CURL_ERROR_SIZE, 1);
		assert(self->error);

		self->request = request;
		retain(request);

		self->session = session;
		retain(session);

		self->completion = completion;

		self->state = TASK_SUSPENDED;
	}

	return self;
}

/**
 * @see URLSessionTaskInterface::resume(URLSessionTask *)
 */
static void resume(URLSessionTask *self) {

	if (self->state == TASK_SUSPENDED) {
		$(self->locals.thread, start);

		self->state = TASK_RUNNING;
	}
}

/**
 * @brief A helper to populate the headers list for CURL.
 */
static BOOL httpHeaders_enumerator(const Dictionary *dictionary, id obj, id key, id data) {

	String *header = $(alloc(String), initWithFormat, "%s: %s",
			((String * ) key)->chars,
			((String * ) obj)->chars);

	struct curl_slist **headers = (struct curl_slist **) data;
	*headers = curl_slist_append(*headers, header->chars);

	release(header);
	return NO;
}

/**
 * @see URLSessionTaskInterface::setup(URLSessionTask *)
 */
static void setup(URLSessionTask *self) {

	self->locals.handle = curl_easy_init();
	assert(self->locals.handle);

	curl_easy_setopt(self->locals.handle, CURLOPT_ERRORBUFFER, self->error);
	curl_easy_setopt(self->locals.handle, CURLOPT_FOLLOWLOCATION, YES);

	Data *body = self->request->httpBody;
	if (body) {
		curl_easy_setopt(self->locals.handle, CURLOPT_POSTFIELDS, body->bytes);
		curl_easy_setopt(self->locals.handle, CURLOPT_POSTFIELDSIZE, body->length);
	}

	struct curl_slist *httpHeaders = NULL;
	const Dictionary *headers = NULL;

	headers = self->session->configuration->httpHeaders;
	if (headers) {
		$(headers, enumerateObjectsAndKeys, httpHeaders_enumerator, &httpHeaders);
	}

	headers = self->request->httpHeaders;
	if (headers) {
		$(headers, enumerateObjectsAndKeys, httpHeaders_enumerator, &httpHeaders);
	}

	curl_easy_setopt(self->locals.handle, CURLOPT_HTTPHEADER, httpHeaders);

	self->locals.httpHeaders = httpHeaders;

	switch (self->request->httpMethod) {
		case HTTP_POST:
			curl_easy_setopt(self->locals.handle, CURLOPT_POST, YES);
			break;
		case HTTP_PUT:
			curl_easy_setopt(self->locals.handle, CURLOPT_PUT, YES);
			break;
		case HTTP_DELETE:
			curl_easy_setopt(self->locals.handle, CURLOPT_CUSTOMREQUEST, "DELETE");
			break;
		case HTTP_HEAD:
			curl_easy_setopt(self->locals.handle, CURLOPT_NOBODY, YES);
			break;
		default:
			break;
	}

	curl_easy_setopt(self->locals.handle, CURLOPT_URL, self->request->url->urlString->chars);
}

/**
 * @see URLSessionTask::teardown(URLSessionTask *)
 */
static void teardown(URLSessionTask *self) {

	if (self->locals.handle) {
		curl_easy_cleanup(self->locals.handle);
	}

	if (self->locals.httpHeaders) {
		curl_slist_free_all(self->locals.httpHeaders);
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->copy = copy;
	object->dealloc = dealloc;

	URLSessionTaskInterface *task = (URLSessionTaskInterface *) clazz->interface;

	task->cancel = cancel;
	task->initWithRequestInSession = initWithRequestInSession;
	task->resume = resume;
	task->setup = setup;
	task->teardown = teardown;
}

Class __URLSessionTask = {
	.name = "URLSessionTask",
	.superclass = &__Object,
	.instanceSize = sizeof(URLSessionTask),
	.interfaceOffset = offsetof(URLSessionTask, interface),
	.interfaceSize = sizeof(URLSessionTaskInterface),
	.initialize = initialize, };

#undef __Class