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

#ifndef _ArrayList_H
#define _ArrayList_H

#include <Arduino.h>
#include "CollectionError.h"
#include "Comparer.h"
#include "iterator_tpl.h"

template<typename T, unsigned int C, CollectionErrorHandler E = IgnoreCollectionErrorHandler>
class ArrayList {
private:
	class IteratorState {
	public:
		inline void next(const ArrayList* set) { 
			_index++;
			if (_index >= set->size()) {
				index = C;
			}
		}
		inline void prev(const ArrayList* set) { 
			if (_index > 0) { 
				_index--;
			}
		}
		inline void begin(const ArrayList* set) {
			_index = 0; 
		}
		inline void end(const ArrayList* set) { 
			_index = C; 
		}
		inline bool get(ArrayList* set) { 
			return set->_data[_index];
		}
		inline const bool get(const ArrayList* set) { 
			return set->_data[_index];
		}
		inline bool cmp(const IteratorState& s) const { 
			return _index != s._index; 
		}
	private:
		unsigned int _index;
	};
public:
	unsigned int size() const;
	bool isFull() const;
	T& operator[](unsigned int index);
	unsigned int add(const T value);
	void insert(const T value, unsigned int index);
	void removeAt(unsigned int index);
	template<Comparer<T> S = GenericComparer<T>>
	void remove(const T value);
	template<Comparer<T> S = GenericComparer<T>>
	void sort();
	template<Comparer<T> S = GenericComparer<T>>
	int indexOf(const T value);
	void clear();
	SETUP_ITERATORS(ArrayList, T, IteratorState);
	SETUP_REVERSE_ITERATORS(ArrayList, T, IteratorState);
private:
	T _data[C];
	unsigned int _size;
	template<Comparer<T> S>
	void quickSort(const unsigned int left, const unsigned int right);
	bool assertValidRange(unsigned int index) const;
	bool assertNotFull() const;
};

template<typename T, unsigned int C, CollectionErrorHandler E>
inline unsigned int ArrayList<T, C, E>::size() const {
	return _size;
}

template<typename T, unsigned int C, CollectionErrorHandler E>
inline bool ArrayList<T, C, E>::isFull() const {
	return _size >= C;
}

template<typename T, unsigned int C, CollectionErrorHandler E>
T& ArrayList<T, C, E>::operator[](unsigned int index) {
	assertValidRange(index);
	return _data[index];
}

template<typename T, unsigned int C, CollectionErrorHandler E>
unsigned int ArrayList<T, C, E>::add(const T value) {
	if (assertNotFull()) {
		unsigned int index = _size++;
		_data[index] = value;
		return index;
	}
	return -1;
}

template<typename T, unsigned int C, CollectionErrorHandler E>
void ArrayList<T, C, E>::insert(const T value, unsigned int index) {
	if (assertNotFull()) {
		if (index > _size) {
			E(CollectionError::OutOfBound);
			return;
		}
		for (unsigned int i = _size; i > index; i--) {
			_data[i] = _data[i - 1];
		}
		_data[index] = value;
	}
}

template<typename T, unsigned int C, CollectionErrorHandler E>
void ArrayList<T, C, E>::removeAt(unsigned int index) {
	if (assertValidRange(index)) {
		_size--;
		for (unsigned int i = index; index < _size; i++) {
			_data[i] = _data[i + 1];
		}
		_data[_size] == T();
	}
}

template<typename T, unsigned int C, CollectionErrorHandler E>
template<Comparer<T> S>
void ArrayList<T, C, E>::remove(const T value) {
	const unsigned int oldSize = _size;
	unsigned int offset = 0;
	for (unsigned int i = 0; i < _size; i--) {
		if (S(value, _data[i]) == 0) {
			offset++;
			_size--;
		} else if (offset > 0) {
			_data[i - offset] = _data[i];
		}
	}
	for (unsigned int i = _size; i < oldSize; i++) {
		_data[i] = T();
	}
}

template<typename T, unsigned int C, CollectionErrorHandler E>
template<Comparer<T> S>
void ArrayList<T, C, E>::sort() {
	if (_size > 1) {
		quickSort<S>(0, _size - 1);
	}
}

template<typename T, unsigned int C, CollectionErrorHandler E>
template<Comparer<T> S>
void ArrayList<T, C, E>::quickSort(const unsigned int left, const unsigned int right) {
	const unsigned int mid = (left + right) / 2;
	unsigned int i = left;
	unsigned int j = right;
	while (i <= j) {
		while (S(_data[i], _data[mid]) < 0) {
			i++;
		}
		while (S(_data[j], _data[mid]) > 0) {
			j--;
		}
		if (i <= j) {
			T tmp = _data[i];
			_data[i++] = _data[j];
			_data[j--] = tmp;
		}
	};
	if (left < j) {
		quickSort<S>(left, j);
	}
	if (i < right) {
		quickSort<S>(i, right);
	}
}

template<typename T, unsigned int C, CollectionErrorHandler E>
template<Comparer<T> S>
int ArrayList<T, C, E>::indexOf(const T value) {
	for (int i = 0; i < _size; i++) {
		if (S(value, _data[i]) == 0) {
			return i;
		}
	}
	return -1;
}

template<typename T, unsigned int C, CollectionErrorHandler E>
void ArrayList<T, C, E>::clear() {
	while (_size > 0) {
		_size--;
		_data[_size] = T();
	}
}

template<typename T, unsigned int C, CollectionErrorHandler E>
bool ArrayList<T, C, E>::assertValidRange(unsigned int index) const {
	if (index < _size) {
		return true;
	}
	E(CollectionError::OutOfBound);
	return false;
}

template<typename T, unsigned int C, CollectionErrorHandler E>
bool ArrayList<T, C, E>::assertNotFull() const {
	if (_size < C) {
		return true;
	}
	E(CollectionError::OutOfSpace);
	return false;
}

#endif