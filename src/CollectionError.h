/*
 * Fixed-Size Collections for Arduino
 *
 * Copyright (c) 2017 Arsène von Wyss. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA * 
 */

#ifndef _CollectionError_H
#define _CollectionError_H

#include <Arduino.h>

enum CollectionError {
	OutOfBound,
	OutOfSpace,
	KeyNotFound,
	DuplicateKey,
	IsEmpty
};

typedef void(*CollectionErrorHandler)(CollectionError);

const char* GetCollectionErrorMessage(CollectionError error) {
	switch (error) {
		case CollectionError::OutOfBound:
			return "Out Of Bound";
		case CollectionError::OutOfSpace:
			return "Out Of Space";
		case CollectionError::KeyNotFound:
			return "Key Not Found";
		case CollectionError::DuplicateKey:
			return "Duplicate Key";
		case CollectionError::IsEmpty:
			return "Is Empty";
		default:
			return "Unknown Error";
	}
}

inline void IgnoreCollectionErrorHandler(CollectionError error) {};

void LogFailCollectionErrorHandler(CollectionError error) {
	Serial.println();
	Serial.print("Collection Error: ");
	Serial.println(GetCollectionErrorMessage(error));
	while (true); // Cause a software watchdog reset; this gives us a nice stack trace
};

#endif