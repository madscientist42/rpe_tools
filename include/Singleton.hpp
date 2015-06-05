/*
 * Singleton.hpp
 *
 * A simplified, largely safe singleton template class.  Uses
 * std::lock_guard (Or, functional equivalent such as from TinyThread++)
 * container to enforce single instance creation at construction time
 * such that if someone attempts to bind to this, it is held off until
 * the class or it's children are constructed with a single global instance.
 *
 * This enforces proper singleton semantics and is effectively thread safe.
 *
 * Care should be exercised in this class' use.  You should AVOID using
 * another Singleton's GetInstance() within the constructor of your
 * definitions.  There's a good chance you'll get a transient of the
 * Singletons effectively blocking on the others.
 *
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

#ifndef SINGLETON_HPP_
#define SINGLETON_HPP_

#include <cassert>

#if defined(USE_TINYTHREAD)
#include "tinythread.h"
using tthread::mutex;
using tthread::lock_guard;
#else
#include <mutex>
using std::mutex;
using std::lock_guard;
#endif

template<class T>
class Singleton
{
public:
	Singleton() {}
	~Singleton() { is_destructed = true; }

	static T* GetInstance()
	{
		assert(!is_destructed);
		(void)is_destructed; // prevent removing is_destructed in Release configuration

		lock_guard<mutex> lock(GetMutex());
		static T instance;
		return &instance;
	}

private:
	static bool is_destructed;

	static mutex& GetMutex()
	{
		static mutex _mutex;
		return _mutex;
	}
};

// Force creating the internal mutex before main() is ever called and
// effectively block anyone trying to grab us before construction is complete
// on this class.  (This means you should use EXTREME care to not have
// a Singleton's constructor relying on ANY other Singleton's GetInstance()
// call- as this might cause a very nasty race condition)
template<class T>
bool Singleton<T>::is_destructed = (Singleton<T>::GetMutex(), false);

#endif /* SINGLETON_HPP_ */
