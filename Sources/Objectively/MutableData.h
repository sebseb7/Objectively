/*
 * Objectively: Ultra-lightweight object oriented framework for GNU C.
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

#pragma once

#include <Objectively/Data.h>

/**
 * @file
 * @brief Mutable data buffers.
 */

typedef struct MutableDataInterface MutableDataInterface;

/**
 * @brief Mutable data buffers.
 * @extends Data
 * @ingroup ByteStreams
 */
struct MutableData {

	/**
	 * @brief The superclass.
	 */
	Data data;

	/**
	 * @brief The interface.
	 * @protected
	 */
	MutableDataInterface *interface;

	/**
	 * @brief The capacity, which is always `>= self->data.length`.
	 * @private
	 */
	size_t capacity;
};

/**
 * @brief The MutableData interface.
 */
struct MutableDataInterface {

	/**
	 * @brief The superclass interface.
	 */
	DataInterface dataInterface;

	/**
	 * @fn void MutableData::appendBytes(MutableData *self, const uint8_t *bytes, size_t length)
	 * @brief Appends the given `bytes` to this Data.
	 * @param self The MutableData.
	 * @param bytes The bytes to append.
	 * @param length The length of bytes to append.
	 * @remarks MutableData are grown in blocks as bytes are appended. This
	 * provides a significant performance gain when frequently appending small
	 * chunks of bytes.
	 * @memberof MutableData
	 */
	void (*appendBytes)(MutableData *self, const uint8_t *bytes, size_t length);

	/**
	 * @fn void MutableData::appendData(MutableData *self, const Data *data)
	 * @brief Appends the given `data` to this Data.
	 * @param self The MutableData.
	 * @param data The Data to append.
	 * @memberof MutableData
	 */
	void (*appendData)(MutableData *self, const Data *data);

	/**
	 * @static
	 * @fn MutableData *MutableData::data(void)
	 * @brief Returns a new MutableData.
	 * @return The new MutableData, or `NULL` on error.
	 * @memberof MutableData
	 */
	MutableData *(*data)(void);

	/**
	 * @static
	 * @fn MutableData *MutableData::dataWithCapacity(size_t capacity)
	 * @brief Returns a new MutableData with the given `capacity`.
	 * @param capacity The desired capacity in bytes.
	 * @return The new MutableData, or `NULL` on error.
	 * @memberof MutableData
	 */
	MutableData *(*dataWithCapacity)(size_t capacity);

	/**
	 * @fn MutableData *MutableData::init(MutableData *self)
	 * @brief Initializes this Data with length `0`.
	 * @param self The MutableData.
	 * @return The initialized Data, or `NULL` on error.
	 * @memberof MutableData
	 */
	MutableData *(*init)(MutableData *self);

	/**
	 * @fn MutableData *MutableData::initWithCapacity(MutableData *self, size_t capacity)
	 * @brief Initializes this Data with the given capacity.
	 * @param self The MutableData.
	 * @param capacity The capacity in bytes.
	 * @return The initialized Data, or `NULL` on error.
	 * @memberof MutableData
	 */
	MutableData *(*initWithCapacity)(MutableData *self, size_t capacity);

	/**
	 * @fn MutableData *MutableData::initWithData(MutableData *self, const Data *data)
	 * @brief Initializes this Data with the contents of `data`.
	 * @param self The MutableData.
	 * @param data A Data.
	 * @return The initialized Data, or `NULL` on error.
	 * @memberof MutableData
	 */
	MutableData *(*initWithData)(MutableData *self, const Data *data);

	/**
	 * @fn void MutableData::setLength(MutableData *self, size_t length)
	 * @brief Sets the length of this Data, truncating or expanding it.
	 * @param self The MutableData.
	 * @param length The new desired length.
	 * @remarks If the data is expanded, the newly allocated bytes are filled
	 * with zeros.
	 * @memberof MutableData
	 */
	void (*setLength)(MutableData *self, size_t length);
};

/**
 * @fn Class *MutableData::_MutableData(void)
 * @brief The MutableData archetype.
 * @return The MutableData Class.
 * @memberof MutableData
 */
OBJECTIVELY_EXPORT Class *_MutableData(void);
