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

#ifndef _HashSet_H
#define _HashSet_H

#include <Arduino.h>
#include "CollectionError.h"
#include "HashComparer.h"
#include "BitSet.h"

template<typename K, unsigned int C, class H = GenericHashComparer<K>, CollectionErrorHandler E = IgnoreCollectionErrorHandler>
class HashSet {
public:
	unsigned int size() const;
	bool isFull() const;
	bool operator[](K key) const;
	bool add(K key);
	bool remove(K key);
	void clear();
	bool tryGetBucket(unsigned int bucket, K& key) const;
private:
	BitSet<C> _used;
	K _keys[C];
	unsigned int _size;
};

template<typename K, unsigned int C, class H, CollectionErrorHandler E>
inline unsigned int HashSet<K, C, H, E>::size() const {
	return _size;
}

template<typename K, unsigned int C, class H, CollectionErrorHandler E>
inline bool HashSet<K, C, H, E>::isFull() const {
	return _size >= C;
}

template<typename K, unsigned int C, class H, CollectionErrorHandler E>
bool HashSet<K, C, H, E>::operator[](K key) const {
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

template<typename K, unsigned int C, class H, CollectionErrorHandler E>
bool HashSet<K, C, H, E>::add(K key) {
	const unsigned int startBucket = H::getHash(key) % C;
	unsigned int bucket = startBucket;
	while (_used[bucket]) {
		if (H::equals(key, _keys[bucket])) {
			return false;
		}
		bucket = (bucket + 1) % C;
		if (bucket == startBucket) {
			E(CollectionError::OutOfSpace);
			return false;
		}
	}
	_used.set(bucket);
	_keys[bucket] = key;
	_size++;
	return true;
}

template<typename K, unsigned int C, class H, CollectionErrorHandler E>
bool HashSet<K, C, H, E>::remove(K key) {
	if (_size > 0) {
		unsigned int bucket = H::getHash(key) % C;
		while (_used[bucket]) {
			if (H::equals(key, _keys[bucket])) {
				_used.unset(bucket);
				_keys[bucket] = K();
				_size--;
				return true;
			}
			bucket++;
		}
	}
	return false;
}

template<typename K, unsigned int C, class H, CollectionErrorHandler E>
void HashSet<K, C, H, E>::clear() {
	for (unsigned int bucket = 0; bucket < C; bucket++) {
		if (_used[bucket]) {
			_keys[bucket] = K();
		}
	}
	_used.clear();
	_size = 0;
}

template<typename K, unsigned int C, class H, CollectionErrorHandler E>
bool HashSet<K, C, H, E>::tryGetBucket(unsigned int bucket, K& key) const {
	if ((bucket < C) && _used[bucket]) {
		key = _keys[bucket];
		return true;
	}
	return false;
}

#endif