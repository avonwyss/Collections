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

#ifndef _HashMap_H
#define _HashMap_H

#include <Arduino.h>
#include "CollectionError.h"
#include "HashComparer.h"
#include "BitSet.h"

template<typename K, typename V, unsigned int C, class H = GenericHashComparer<K>, CollectionErrorHandler E = IgnoreCollectionErrorHandler>
class HashMap {
public:
	unsigned int capacity() const;
	unsigned int size() const;
	bool isFull() const;
	bool containsKey(K key) const;
	bool tryGet(K key, V& value) const;
	V operator[](K key) const;
	void add(K key, V value);
	void set(K key, V value);
	bool remove(K key);
	void clear();
	bool tryGetBucket(unsigned int bucket, K& key, V& value) const;
private:
	BitSet<C> _used;
	K _keys[C];
	V _values[C];
	unsigned int _size;
};

template<typename K, typename V, unsigned int C, class H, CollectionErrorHandler E>
inline unsigned int HashMap<K, V, C, H, E>::capacity() const {
	return C;
}

template<typename K, typename V, unsigned int C, class H, CollectionErrorHandler E>
inline unsigned int HashMap<K, V, C, H, E>::size() const {
	return _size;
}

template<typename K, typename V, unsigned int C, class H, CollectionErrorHandler E>
inline bool HashMap<K, V, C, H, E>::isFull() const  {
	return _size >= C;
}

template<typename K, typename V, unsigned int C, class H, CollectionErrorHandler E>
bool HashMap<K, V, C, H, E>::containsKey(K key) const  {
	if (_size > 0) {
		unsigned int bucket = H::getHash(key) % C;
		while (_used[bucket]) {
			if (H::equals(key, _keys[bucket])) {
				return true;
			}
			bucket++;
		}
	}
	return false;
}

template<typename K, typename V, unsigned int C, class H, CollectionErrorHandler E>
bool HashMap<K, V, C, H, E>::tryGet(K key, V& value) const  {
	if (_size > 0) {
		unsigned int bucket = H::getHash(key) % C;
		while (_used[bucket]) {
			if (H::equals(key, _keys[bucket])) {
				value = _values[bucket];
				return true;
			}
			bucket++;
		}
	}
	return false;
}

template<typename K, typename V, unsigned int C, class H, CollectionErrorHandler E>
V HashMap<K, V, C, H, E>::operator[](K key) const {
	V value;
	if (!tryGet(key, &value)) {
		E(CollectionError::KeyNotFound);
	}
	return value;
}

template<typename K, typename V, unsigned int C, class H, CollectionErrorHandler E>
void HashMap<K, V, C, H, E>::add(K key, V value) {
	if (isFull()) {
		E(CollectionError::OutOfSpace);
		return;
	}
	unsigned int bucket = H::getHash(key) % C;
	while (_used[bucket]) {
		if (H::equals(key, _keys[bucket])) {
			E(CollectionError::DuplicateKey);
			return;
		}
		bucket = (bucket + 1) % C;
	}
	_used.set(bucket);
	_keys[bucket] = key;
	_size++;
	_values[bucket] = value;
}

template<typename K, typename V, unsigned int C, class H, CollectionErrorHandler E>
void HashMap<K, V, C, H, E>::set(K key, V value) {
	const unsigned int startBucket = H::getHash(key) % C;
	unsigned int bucket = startBucket;
	while (_used[bucket]) {
		if (H::equals(key, _keys[bucket])) {
			_values[bucket] = value;
			return;
		}
		bucket = (bucket + 1) % C;
		if (bucket == startBucket) {
			E(CollectionError::OutOfSpace);
			return;
		}
	}
	_used.set(bucket);
	_keys[bucket] = key;
	_size++;
	_values[bucket] = value;
}

template<typename K, typename V, unsigned int C, class H, CollectionErrorHandler E>
bool HashMap<K, V, C, H, E>::remove(K key) {
	if (_size > 0) {
		unsigned int bucket = H::getHash(key) % C;
		while (_used[bucket]) {
			if (H::equals(key, _keys[bucket])) {
				_keys[bucket] = K();
				_values[bucket] = V();
				_used.unset(bucket);
				_size--;
				return true;
			}
			bucket++;
		}
	}
	return false;
}

template<typename K, typename V, unsigned int C, class H, CollectionErrorHandler E>
void HashMap<K, V, C, H, E>::clear() {
	for (unsigned int bucket = 0; bucket < C; bucket++) {
		if (_used[bucket]) {
			_keys[bucket] = K();
			_values[bucket] = V();
		}
	}
	_used.clear();
	_size = 0;
}

template<typename K, typename V, unsigned int C, class H, CollectionErrorHandler E>
bool HashMap<K, V, C, H, E>::tryGetBucket(unsigned int bucket, K& key, V& value) const {
	if ((bucket < C) && _used[bucket]) {
		key = _keys[bucket];
		value = _values[bucket];
		return true;
	}
	return false;
}

#endif