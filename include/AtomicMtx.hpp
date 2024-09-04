/*
 * AtomicMtx.hpp
 *
 * Adds simplistic Atomic mutex-like locking system for tight
 * timing situations.  Lock action is considerably faster than
 * a mutex with provisos with only minor downsides.
 *
 * Copyright (c) 2013-2024 Frank C. Earl, All Rights Reserved.
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

#pragma once

#include <atomic>
using std::atomic_flag;
#include <mutex>            // Just for lock_guard...
using std::lock_guard;
#include <thread>
using std::this_thread::yield;
#include <NONCOPY.hpp>


/*
	Adding a notion of a bit of an Atomic operations mutex.  This, on modern
	CPUs for systems (Embedded and otherwise...) is guaranteed with a properly
	compliant C++ compiler to be lock-free and single cycle execution.  Coupled
	with a good macro to quickly stage a grab for the lock that spins with a
	yield() when called and when the variable defined by the macro leaves scope
	it unlocks for you so you only need to worry about LOCKING.

	It has some provisos.  The main one is that you can't re-grab a lock.  Not
	that was a good idea or practice to begin with...  Just...don't.
*/
class AtomicMtx : public NONCOPY
{
    public:
        AtomicMtx() : flag(ATOMIC_FLAG_INIT) {};
        // Test and set the lock to be true.  If we had it already set, we didn't get
        // the lock.  You only get to lock this *ONCE*- no reacquistion of this lock.
        void lock() { while (flag.test_and_set(std::memory_order_acquire)) { yield(); }; };
        void unlock() { flag.clear(); };

    private:
        atomic_flag flag;
};

#define ATOMIC_LCK(X) lock_guard<AtomicMtx> X_lck(X)