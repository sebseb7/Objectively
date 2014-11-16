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

#include <check.h>

#include <Objectively.h>

START_TEST(string)
	{
		String *string = $(String, alloc(String), initWithFormat, "hello");

		ck_assert(string);
		ck_assert_ptr_eq(&__String, classof(string));

		ck_assert_str_eq("hello", string->str);

		$(String, string, appendFormat, " %s", "world!");
		ck_assert_str_eq("hello world!", string->str);

		String *copy = (String *) $(Object, string, copy);

		ck_assert($(Object, copy, isEqual, (Object *) string));
		ck_assert_int_eq($(Object, copy, hash), $(Object, string, hash));

		release(copy);

		$(String, string, appendString, string);
		ck_assert_str_eq("hello world!hello world!", string->str);

		String *prefix = $(String, alloc(String), initWithFormat, "hello");
		ck_assert($(String, string, hasPrefix, prefix));

		String *suffix = $(String, alloc(String), initWithFormat, "world!");
		ck_assert($(String, string, hasSuffix, suffix));

		RANGE range = { 6, 11 };
		String *substring = $(String, string, substring, range);
		ck_assert_str_eq("world!hello", substring->str);

		release(substring);
		release(prefix);
		release(suffix);
		release(string);

	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("string");
	tcase_add_test(tcase, string);

	Suite *suite = suite_create("string");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
