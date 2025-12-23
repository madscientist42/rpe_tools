/*
 * Singleton.hpp
 *
 * A Thread-safe-ish Singleton class.  Redone from the original to keep the same API.
 * This uses the Meyers model and should be intrinsically safe for most uses.  Care
 * should be taken to avoid a dependency loop in Singletons.  This will cause you 
 * to have a race condition in your program and therefore a segfault.
 */
#pragma once

#include <atomic>
#include <mutex>
#include <NONCOPY.hpp>

template<class T> class Singleton : public NONCOPY
{
public:
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
};

