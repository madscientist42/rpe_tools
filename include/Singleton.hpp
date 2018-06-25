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
#include <tinythread.h>
using tthread::mutex;
using tthread::lock_guard;
using tthread::atomic_thread_fence;
using tthread::memory_order_acquire;
using tthread::memory_order_release;
#elif defined (USE_BOOST)
#include <boost/thread/mutex.hpp>
using boost::mutex;
using boost::lock_guard;
#include <boost/atomic.hpp>
using boost::atomic_thread_fence;
using boost::memory_order_acquire;
using boost::memory_order_release;
#else
#include <mutex>
using std::mutex;
using std::lock_guard;
using std::atomic_thread_fence;
using std::memory_order_acquire;
using std::memory_order_release;
#endif

#include <NONCOPY.hpp>

template<class T> class Singleton : public NONCOPY 
{
public:
	Singleton() {}
	~Singleton()
	{
		is_destructed = true;
		if (is_constructed)
		{
			delete GetInstance();		// Yes, yes, I *KNOW*.  Bad programmer, no twinkie...
		}
	}

	static bool isConstructed() { return is_constructed; }

	static T* GetInstance()
	{
		static T *instance = NULL;

		assert(!is_destructed);

		(void)is_destructed; // prevent removing is_destructed in Release configuration
		(void)is_constructed; // prevent removing is_constructed in Release configuration

		if (instance == NULL)
		{
			// Bracket this with a memory fence to FORCE multiple threads on other CPUs
			// will be in sync with each other and the check for NULL on the constructor
			// allocation attempt will be only done by ONE thread.
			lock_guard<mutex> lock(GetMutex());
			atomic_thread_fence(memory_order_acquire);
			if (instance == NULL)
			{
				// Allocate and construct an instance of ourselves or our child...
				instance = new T();
				is_constructed = true;
			}
			atomic_thread_fence(memory_order_release);
		}
		return instance;
	}

private:
	static bool is_destructed;
	static bool is_constructed;

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
template<class T>
bool Singleton<T>::is_constructed = (Singleton<T>::GetMutex(), false);

#endif /* SINGLETON_HPP_ */
