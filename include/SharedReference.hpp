 /*
 * SharedReference.hpp
 *
 * A simple container/shield to use references through STL without
 * slicing your objects all up because of implicit object copies
 * getting done on you.  It is an encapsulation of a shared_ptr
 * that does several implicit off-heap memory allocations or
 * accepts by ref/ptr allocations from other parties.  This is
 * an RAII container.  ONCE YOU HAND THIS A REFERENCE OR A POINTER,
 * THE CONTAINING shared_ptr AND ALL OF IT'S COPIES OWNS THAT MEMORY,
 * WITH THE LAST SharedReference DOING THE DESTRUCTION OF THE MEMORY
 * ALLOCATION.
 *
 * Copyright (c) 2014, 2015 Frank C. Earl
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

#ifndef _SHAREDREFERENCE_HPP_
#define _SHAREDREFERENCE_HPP_

// NOTE: This *requires* C++11 compliance in your compiler to use this configuration.
#include <memory>
using std::shared_ptr;
using std::addressof;

#include <queue>
using std::queue;

template <typename T>
class SharedReference
{
public:
	SharedReference()  		{ SharedReference::reset(new T()); };		// In order to make containers like std::map happy, you have to do this...
	SharedReference(T &ref)	{ SharedReference::reset(ref); };
	SharedReference(T *ptr)	{ SharedReference::reset(ptr); };
	template<typename U>
		SharedReference(shared_ptr<U> &sp_ref) { SharedReference::reset(sp_ref); };

	~SharedReference() {};

	// Re-setters - if you pass a value or reference of the specified type, it will
	// 				do a copy into the
	void reset(T &ref)	{ _ptr.reset(addressof(ref)); };		// *Must* assume it's safe to enclose this.
	void reset(T *ptr)	{ _ptr.reset(ptr); };					// *Must* assume it's safe to enclose this.
	template<typename U>
		void reset(shared_ptr<U> &sp_ref) { _ptr = sp_ref; };	// Just copy the smart pointer to ours...

	// Now, expose a ref.  If you use it directly off this object, you're 100%
	// safe regarding it's use.  You're in scope, so the object so allocated
	// by the copy/allocator constructors from an originating SmartRef because
	// it's relative
	operator T&()	    { return *_ptr; };
	T& get()  			{ return *_ptr; };
	T& operator *()		{ return *_ptr; };

	// Specialized assignment operators.  These handle the special cases that
	// the compiler default behaviors would not cover correctly without explicit
	// overrides.
	SharedReference& operator=(T &ref) { SharedReference::reset(ref); return *this; };
	SharedReference& operator=(T *ptr) { SharedReference::reset(ptr); return *this; };
	template<typename U>
		SharedReference& operator=(shared_ptr<U> &sp_ref) { SharedReference::reset(sp_ref); return *this; };

private:
	shared_ptr<T>	  _ptr;
};


#endif /* _SHAREDREFERENCE_HPP_ */
