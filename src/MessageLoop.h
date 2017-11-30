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

#ifndef _MessageLoop_H
#define _MessageLoop_H

#include <Arduino.h>
#include <user_interface.h>
#include "Deque.h"
#include "CollectionError.h"

template <unsigned int C, CollectionErrorHandler E = IgnoreCollectionErrorHandler>
class MessageLoop {
public:
	static constexpr unsigned int CSIZE = 4;
	void post(int (*callbackFcn)(), int delayMs = 0);
	template <typename T>
	void post(int (*callbackFcn)(T*&), T* pData, int delayMs = 0);
	void post(int (*callbackFcn)(bool&), bool bData, int delayMs = 0);
	void post(int (*callbackFcn)(int&), int iData, int delayMs = 0);
	void post(int (*callbackFcn)(unsigned int&), unsigned int iData, int delayMs = 0);
	void process();
private:
	union MessageData {
		void* pData;
		int iData;
		int(*fData)();
	};
	struct Message {
		int (*callbackFcn)(MessageData&);
		unsigned long tick;
		MessageData data;
	};
	static int invokeVoidFunction(MessageData& data);
	Deque<Message, C, E> _messages;
};

template <unsigned int C, CollectionErrorHandler E>
int MessageLoop<C, E>::invokeVoidFunction(MessageData& data) {
	return data.fData();
}

template <unsigned int C, CollectionErrorHandler E>
void MessageLoop<C, E>::post(int(*callbackFcn)(), int delayMs) {
	Message msg;
	msg.callbackFcn = MessageLoop<C, E>::invokeVoidFunction;
	msg.data.fData = callbackFcn;
	msg.tick = millis() + delayMs;
	_messages.push(msg);
}

template <unsigned int C, CollectionErrorHandler E>
template <typename T>
void MessageLoop<C, E>::post(int (*callbackFcn)(T*&), T* pData, int delayMs) {
	Message msg;
	msg.callbackFcn = (int (*)(MessageData&))callbackFcn;
	msg.data.pData = pData;
	msg.tick = millis() + delayMs;
	_messages.push(msg);
}

template <unsigned int C, CollectionErrorHandler E>
void MessageLoop<C, E>::post(int (*callbackFcn)(int&), int iData, int delayMs) {
	Message msg;
	msg.callbackFcn = (int (*)(MessageData&))callbackFcn;
	msg.data.iData = iData;
	msg.tick = millis() + delayMs;
	_messages.push(msg);
}
	
template <unsigned int C, CollectionErrorHandler E>
void MessageLoop<C, E>::post(int (*callbackFcn)(bool&), bool bData, int delayMs) {
	Message msg;
	msg.callbackFcn = (int (*)(MessageData&))callbackFcn;
	msg.data.iData = bData;
	msg.tick = millis() + delayMs;
	_messages.push(msg);
}
	
template <unsigned int C, CollectionErrorHandler E>
void MessageLoop<C, E>::post(int (*callbackFcn)(unsigned int&), unsigned int iData, int delayMs) {
	Message msg;
	msg.callbackFcn = (int (*)(MessageData&))callbackFcn;
	msg.data.iData = (int)iData;
	msg.tick = millis() + delayMs;
	_messages.push(msg);
}
	
template <unsigned int C, CollectionErrorHandler E>
void MessageLoop<C, E>::process() {
	Message msg;
	while (_messages.tryShift(msg)) {
		if (msg.tick <= millis()) {
			int result = msg.callbackFcn(msg.data);
			if (result <= 0) {
				break;
			}
			msg.tick = millis() + result;
		}
		_messages.push(msg);
		break;
	}
}

#endif