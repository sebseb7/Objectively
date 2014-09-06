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

#ifndef _objectively_object_h_
#define _objectively_object_h_

#include <objectively/class.h>
#include <objectively/types.h>

/*
 * @brief Object is the root instance type. Every subclass of Object must
 * declare a structure that begins with a `struct Object`.
 */
struct Object {

	/*
	 * @brief Every instance of Object begins with a pointer to its Class.
	 */
	const Class *class;

	/*
	 * @brief Creates a shallow copy of this Object.
	 *
	 * @param self The instance.
	 *
	 * @return The copy.
	 */
	id (*copy)(const id self);

	/*
	 * @brief Frees all resources held by this Object.
	 */
	void (*dealloc)(id self);

	/*
	 * @brief Tests equality of the other Object.
	 *
	 * @return YES if other is deemed equal, NO otherwise.
	 */
	BOOL (*isEqual)(const id self, const id other);

	/*
	 * @brief Tests for class hierarchy membership.
	 *
	 * @return YES if this instance belongs to class' hierarchy, NO otherwise.
	 */
	BOOL (*isKindOfClass)(const id self, const Class *class);

	/*
	 * @brief The initializer is responsible for populating newly allocated
	 * instances with valid state and behavior.
	 *
	 * @details This method typically begins with a call to the superclass
	 * initializer, passing the arguments list up the initializer chain. Method
	 * overrides as well as method and member assignment and initialization
	 * then follow.
	 *
	 * @param self The newly allocated instance.
	 * @param args The initializer arguments list.
	 *
	 * @return The initialized instance, typically self.
	 */
	id (*init)(id self, va_list *args);
};

extern const Class *Object;

#endif