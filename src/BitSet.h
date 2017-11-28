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

#ifndef _BitSet_H
#define _BitSet_H

#include <Arduino.h>
#include "CollectionError.h"
#include "iterator_tpl.h"

template<unsigned int C, CollectionErrorHandler E = IgnoreCollectionErrorHandler>
class BitSet {
private:
	class IteratorState {
	public:
		inline void next(const BitSet* set) { 
			if (_index < C) { 
				_index++;
			}
		}
		inline void prev(const BitSet* set) { 
			if (_index > 0) { 
				_index--;
			}
		}
		inline void begin(const BitSet* set) {
			_index = 0; 
		}
		inline void end(const BitSet* set) { 
			_index = C; 
		}
		inline bool get(BitSet* set) { 
			return set->operator[](_index);
		}
		inline const bool get(const BitSet* set) { 
			return set->operator[](_index);
		}
		inline bool cmp(const IteratorState& s) const { 
			return _index != s._index; 
		}
	private:
		unsigned int _index;
	};
public:
	unsigned int size() const;
	bool operator [](const unsigned int i) const;
	void set(const unsigned int i);
	void unset(const unsigned int i);
	void clear();
	SETUP_ITERATORS(BitSet, bool, IteratorState);
	SETUP_REVERSE_ITERATORS(BitSet, bool, IteratorState);
private:
	static constexpr unsigned int BITS = sizeof(unsigned int)*8;
	static constexpr unsigned int MASK = BITS - 1;
	unsigned int _data[(C+MASK)/BITS];
	bool assertValidRange(const unsigned int i) const;
};

template<unsigned int C, CollectionErrorHandler E>
inline unsigned int BitSet<C, E>::size() const {
	return C;
}

template<unsigned int C, CollectionErrorHandler E>
inline bool BitSet<C, E>::assertValidRange(const unsigned int i) const {
	if (i < C) {
		return true;
	}
	E(CollectionError::OutOfBound);
	return false;
}

template<unsigned int C, CollectionErrorHandler E>
inline bool BitSet<C, E>::operator[](const unsigned int i) const {
	if (assertValidRange(i)) {
		return (_data[i / BITS] & (1 << (i & MASK))) > 0;
	}
	return false;
}

template<unsigned int C, CollectionErrorHandler E>
void BitSet<C, E>::set(const unsigned int i) {
	if (assertValidRange(i)) {
		_data[i / BITS] |= (1 << (i & MASK));
	}
}

template<unsigned int C, CollectionErrorHandler E>
void BitSet<C, E>::unset(const unsigned int i) {
	if (assertValidRange(i)) {
		_data[i / BITS] &= ~(1 << (i & MASK));
	}
}

template<unsigned int C, CollectionErrorHandler E>
void BitSet<C, E>::clear() {
	memset(&_data[0], 0, sizeof(_data));
}

#endif