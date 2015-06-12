 /*
 * MessageManager.hpp
 *
 * A simple sparse templated space mailbox slot class.  This allows the
 * creation of a mailbox slot queue system for concurrent accesses.
 * It accepts a slot value of 0-><maxint> (Though it isn't reccomended
 * to try for all of that. Memory won't let you...) and creates a
 * map entry for a new queue if it isn't present and pushes to the
 * queue for the slot index specified.
 *
 * Copyright (c) 2013, 2014, 2015 Frank C. Earl
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions, and the following disclaimer.  You may add your
 *    own copyright notice relative to your modifications, but you cannot claim
 *    the code herein as solely your own.
 *
 * 2. Binary redistributions must reproduce the above copyright notice, either
 *    in the initial output of the derived application, a "help" screen, or in
 *    the documentation that accompanies the same.
 *
 * 3. Neither the name of the copyright holder nor the names of this software's
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.  Compliance with
 *    condition 2 does not constitute a violation of this condition.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef INCLUDE_MESSAGEMANAGER_HPP_
#define INCLUDE_MESSAGEMANAGER_HPP_

#include <string>
using std::string;

#include <map>
using std::pair;
using std::map;
#include <queue>
using std::queue;

#if defined(USE_TINYTHREAD)
#include "tinythread.h"
using tthread::mutex;
using tthread::lock_guard;
#else
#include <mutex>
using std::mutex;
using std::lock_guard;
#endif

#include "Singleton.hpp"

template<typename T>
class MessageManager : public Singleton<MessageManager>
{
public:
	MessageManager() : _maxSlots(50) {};
	virtual ~MessageManager() {};

	// Size management methods...
	bool setMaxSlots(uint32_t maxSlots)
	{
		if ((maxSlots > 0) && (maxSlots < 1500))
		{
			_maxSlots = maxSlots;
		}
		return (_maxSlots == maxSlots);
	};

	uint32_t getMaxSlots(void) { return _maxSlots; };

	// Message sending/receiving methods.
	bool sendMessage(int slot, const T &msg)
	{
		bool retVal = false;

		// Note: Sending a message for a new slot adds it to the container here
		//       so long as we don't exceed the number of slots that the
		// 		 initializer specifies- this call will only really return false
		// 		 when someone trys to exceed that set count.
		if (_mailbox.size() <= _maxSlots)
		{
			retVal = true;
			lock_guard<mutex> msg_lock(_mailbox[slot]._lock);
			_mailbox[slot]._queue.push(msg);
			// Mutex is released as soon as we leave scope here...
		}

		return retVal;
	}

	bool getMessage(int slot, T &msg)
	{
		bool retVal = false;

		if (checkSlot(slot))
		{
			// We have it defined...
			if (!_mailbox[slot]._queue.empty())
			{
				// Fetch out an entry from the queue...
				lock_guard<mutex> msg_lock(_mailbox[slot]._lock);
				retVal = true;
				msg = _mailbox[slot]._queue.front();
				_mailbox[slot]._queue.pop();
				// Mutex is released as soon as we leave scope here...
			}
		}
	}

	return retVal;

private:
	typedef queue<T> msg_queue;
	typedef struct
	{
		mutex		_lock;
		msg_queue	_queue;
	} mailbox_queue;
	typedef pair<const int, mailbox_queue> mailbox_slot;
	typedef map<const int, mailbox_queue> mailbox;

	uint32_t 		_maxSlots;
	mailbox			_mailbox;


	bool		checkSlot(int slot) { return _mailbox.find(slot) != _mailbox.end(); };
};

#endif /* INCLUDE_MESSAGEMANAGER_HPP_ */
