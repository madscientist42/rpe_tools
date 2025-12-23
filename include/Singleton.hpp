/*
 * Singleton.hpp
 *
<<<<<<< HEAD
 * A simplified, largely safe singleton template class.  Uses
 * std::lock_guard container to enforce single instance creation
 * semantics at construction time such that if someone attempts to
 * bind to this, it is held off until the class or it's children
 are constructed with a single global instance.
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
=======
 * A Thread-safe-ish Singleton class.  Redone from the original to keep the same API.
 * This uses the Meyers model and should be intrinsically safe for most uses.  Care
 * should be taken to avoid a dependency loop in Singletons.  This will cause you 
 * to have a race condition in your program and therefore a segfault.
>>>>>>> b52e304 (Cruft removal and other cleanups.)
 */
#pragma once

#include <atomic>
#include <mutex>
#include <NONCOPY.hpp>

template<class T> class Singleton : public NONCOPY
{
public:
<<<<<<< HEAD
    /// Default constructor
	Singleton() {}

	/// Destructor.
	///
	/// This destructor is responsible for cleaning up the singleton after it's been
	/// constructed.  It is necessary because the instance is static and the C++ runtime
	/// will not call the destructor for static objects.  It is also necessary because the
	/// destructor is responsible for deleting the singleton instance.  See the comments
	/// in the implementation for more details.
	~Singleton()
	{
		is_destructed = true;
		if (is_constructed)
		{
			delete GetInstance();		// Yes, yes, I *KNOW*.  Bad programmer, no twinkie...
		}
	}

    /// Returns true if the instance has been constructed and false otherwise
	static bool isConstructed() { return is_constructed; }

    /**
     * Retrieves the singleton instance of type T.
     *
     * This method employs a thread-safe mechanism to ensure that the singleton instance
     * is created only once during the program's lifetime. It utilizes a double-checked
     * locking pattern combined with memory fences to synchronize access across multiple
     * threads. If the instance is not yet created, a mutex is used to lock the critical
     * section, ensuring only one thread can allocate and construct the instance. Once
     * created, the instance is returned for subsequent calls, ensuring consistent and
     * efficient access.
     *
     * @return A pointer to the singleton instance of type T.
     */
	static T* GetInstance()
	{
		static T *instance = NULL;

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

    /*
    * Returns the mutex used to synchronize access to the singleton instance.
    *
    * This natty bit of code is used here to make the compiler do this the, "right" way without
    * us having to worry about a C/C++ file to drop the static instance into.  Definition makes
    * it simply happen as a header-only affair.
    */
	static mutex& GetMutex()
	{
		static mutex _mutex;
		return _mutex;
	}
=======
	/// Return a pointer to the single instance of this class.
	///
	/// This function will return a pointer to the single instance of this class.
	/// The instance is created on the first call to this function and is
	/// destroyed when the program exits.  This is a thread-safe way to get
	/// a Singleton instance of a class.
	///
	/// @return A pointer to the single instance of this class.
    static T* GetInstance()
    {
		static T instance;
		return &instance;
    }
>>>>>>> b52e304 (Cruft removal and other cleanups.)
};

