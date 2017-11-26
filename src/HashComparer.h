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

#ifndef _HashComparer_H
#define _HashComparer_H

#include <Arduino.h>

class StringHashComparer {
public:
	static unsigned int getHash(String value);
	static bool equals(String x, String y);
private:
	StringHashComparer() {}
};

class StringIgnoreCaseHashComparer {
public:
	static unsigned int getHash(String value);
	 static bool equals(String x, String y);
private:
	StringIgnoreCaseHashComparer() {}
};

template <typename K>
class GenericHashComparer {
public:
	static unsigned int getHash(const K value);
	static bool equals(const K x, const K y);
private:
	GenericHashComparer() {}
};

template <typename K>
inline unsigned int GenericHashComparer<K>::getHash(const K value) {
	return (unsigned int)value;
}

template <typename K>
inline bool GenericHashComparer<K>::equals(const K x, const K y) {
	return x == y;
}

unsigned int StringHashComparer::getHash(const String value) {
	unsigned int h = 37;
	for (int i = std::min(31, (int)(value.length()-1)); i >= 0; i--) {
		h = (h * 54059) ^ (value.charAt(i) * 76963);
	}
	return h; // or return h % C;
}

bool StringHashComparer::equals(const String x, const String y) {
	return x.equals(y);
}

unsigned int StringIgnoreCaseHashComparer::getHash(const String value) {
	unsigned int h = 37;
	for (int i = std::min(31, (int)(value.length()-1)); i >= 0; i--) {
		// rough uppercase computation is good enough for hashing
		h = (h * 54059) ^ ((value.charAt(i) & 0xDF) * 76963); 
	}
	return h; // or return h % C;
}

bool StringIgnoreCaseHashComparer::equals(const String x, const String y) {
	return x.equalsIgnoreCase(y);
}

#endif