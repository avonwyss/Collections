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

#ifndef _Deque_H
#define _Deque_H

#include <Arduino.h>
#include "CollectionError.h"
#include "iterator_tpl.h"

template<typename T, unsigned int C, CollectionErrorHandler E = IgnoreCollectionErrorHandler>
class Deque {
private:
	static constexpr unsigned int SLOTS = C + 1;
	class IteratorState {
	public:
		inline void next(const Deque* deque) {
			if (_index != deque->_head) {
				_index = (_index + 1) % SLOTS;
			}
		}
		inline void prev(const Deque* deque) { 
			if (_index != deque->_tail) {
				_index = (_index + SLOTS - 1) % SLOTS;
			}
		}
		inline void begin(const Deque* deque) {
			_index = deque->_tail; 
		}
		inline void end(const Deque* deque) { 
			_index = deque->_head; 
		}
		inline T& get(Deque* deque) { 
			return deque->_data[_index];
		}
		inline const T& get(const Deque* deque) { 
			return deque->_data[_index];
		}
		inline bool cmp(const IteratorState& s) const { 
			return _index != s._index; 
		}
	private:
		unsigned int _index;
	};
public:
	unsigned int capacity() const;
	unsigned int size() const;
	bool isFull() const;
	bool isEmpty() const;
	bool push(T value);
	T pop();
	bool tryPop(T& value);
	bool unshift(T value);
	T shift();
	bool tryShift(T& value);
	T peekFront() const;
	T peekBack() const;
	SETUP_ITERATORS(Deque, T&, IteratorState);
	SETUP_REVERSE_ITERATORS(Deque, T&, IteratorState);
private:
	T _data[SLOTS];
	unsigned int _head;
	unsigned int _tail;
	bool assertNotFull() const;
	bool assertNotEmpty() const;
};

template<typename T, unsigned int C, CollectionErrorHandler E>
inline unsigned int Deque<T, C, E>::capacity() const {
	return C;
}

template<typename T, unsigned int C, CollectionErrorHandler E>
inline unsigned int Deque<T, C, E>::size() const {
	return _head >= _tail ? _tail - _head : SLOTS - _tail + _head;
}

template<typename T, unsigned int C, CollectionErrorHandler E>
inline bool Deque<T, C, E>::isFull() const {
	return (_head + 1) % SLOTS == _tail;
}

template<typename T, unsigned int C, CollectionErrorHandler E>
inline bool Deque<T, C, E>::isEmpty() const {
	return _tail == _head;
}

template<typename T, unsigned int C, CollectionErrorHandler E>
bool Deque<T, C, E>::push(T value) {
	if (assertNotFull()) {
		_data[_head] = value;
		_head = (_head + 1) % SLOTS;
		return true;
	}
	return false;
}

template<typename T, unsigned int C, CollectionErrorHandler E>
T Deque<T, C, E>::pop() {
	if (assertNotEmpty()) {
		_head = _head == 0 ? SLOTS - 1 : _head - 1;
		return _data[_head];
	}
	return T();
}

template<typename T, unsigned int C, CollectionErrorHandler E>
bool Deque<T, C, E>::tryPop(T& value) {
	if (isEmpty()) {
		return false;
	}
	_head = _head == 0 ? SLOTS - 1 : _head - 1;
	value = _data[_head];
	return true;
}

template<typename T, unsigned int C, CollectionErrorHandler E>
bool Deque<T, C, E>::unshift(T value) {
	if (assertNotFull()) {
		_tail = _tail == 0 ? SLOTS - 1 : _tail - 1;
		_data[_tail] = value;
		return true;
	}
	return false;
}

template<typename T, unsigned int C, CollectionErrorHandler E>
T Deque<T, C, E>::shift() {
	if (assertNotEmpty()) {
		const unsigned int tail = _tail;
		_tail = (_tail + 1) % SLOTS;
		return _data[tail];
	}
	return T();
}

template<typename T, unsigned int C, CollectionErrorHandler E>
bool Deque<T, C, E>::tryShift(T& value) {
	if (isEmpty()) {
		return false;
	}
	value = _data[_tail];
	_tail = (_tail + 1) % SLOTS;
	return true;
}

template<typename T, unsigned int C, CollectionErrorHandler E>
T Deque<T, C, E>::peekFront() const {
	if (assertNotEmpty()) {
		return _data[_head == 0 ? SLOTS - 1 : _head - 1];
	}
	return T();
}

template<typename T, unsigned int C, CollectionErrorHandler E>
T Deque<T, C, E>::peekBack() const {
	if (assertNotEmpty()) {
		return _data[_tail];
	}
	return T();
}

template<typename T, unsigned int C, CollectionErrorHandler E>
inline bool Deque<T, C, E>::assertNotFull() const {
	if (isFull()) {
		E(CollectionError::OutOfSpace);
		return false;
	}
	return true;
}

template<typename T, unsigned int C, CollectionErrorHandler E>
inline bool Deque<T, C, E>::assertNotEmpty() const {
	if (isEmpty()) {
		E(CollectionError::IsEmpty);
		return false;
	}
	return true;
}
#endif