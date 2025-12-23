/*
 * Runable.h
 *
 * This is a simplistic base class to provide some, but not all of the
 * functionality of Java's "Runable" interface.  It utilizes a C++ 11
 * Standard model/semantic and operates off of the semantics of the same.
 * We provide, as part of the implementation, the ability to use TinyThread
 * or Boost::thread instead of the stdc++ implementation, in the event
 * that your version on your target is "broken" and does wrong things
 * with this (and there has been some C++ compilers that produced bad
 * code for this over time...)
 *
 * This code does not require the use of C++11 features to accomplish
 * this even though it's use is the same of that.
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

#pragma once

#include <functional>
#include <exception>

// Provide the hooks for the sleep() method abstraction...
#if defined(_WIN32)
#include <windows.h>
#else
#include <poll.h>
#endif

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
using std::thread;
using std::mutex;
using std::recursive_mutex;
using std::condition_variable;
using std::lock_guard;
using std::atomic;
using std::this_thread::sleep_for;
using std::this_thread::yield;
using std::chrono::milliseconds;

#include <NONCOPY.hpp>

#include <stdio.h>

/*
 * Some notes:
 *
 * 		- This class is a *thin* encapsulation shim around the C++11
 * 		  standard thread framework or a compatible implementation of the same.
 * 		  (See TinyThread++...which is just that under a BSD license...)
 * 		- This class creates a "Runable" object with ::run() as the thread
 * 		  loop method.  This is the same base semantics as the Java Runable interface
 * 		  or the Thread Class (for C++ the concepts lean towards the interface, but...).
 * 		- This allows us to make autonomous modules that can be standalone
 * 		  applications or submodules inside of a main control daemon...
 * 		- This class handles only *one* distinct thread of execution.  You want
 * 		  to crack your class definition down to individual threads
 * 		  that inherit this class and then encapsulate or inherit from those, or
 * 		  learn to drive the full thing and DIY if more than that is needed.
 */


class Runable : public NONCOPY
{
public:
    /// Default constructor
	Runable() : _thread(NULL), _run(false) {} ;
	

	/**
	 * Destructor for the Runable class.
	 *
	 * This destructor is special because it tries to clean up after a thread
	 * that was created by the Runable class.  If an exception is thrown
	 * during the cleanup process, catch it and print the error message.
	 */
    virtual ~Runable()
    {
    	try
    	{
    		stop();
    		join();
    		delete _thread;
    	}
    	catch(std::exception& e)
    	{
    		printf("Runable : %s\n", e.what());
    	}
    };

	/**
	 * Joins the thread object.
	 *
	 * This function will join the thread object to the main thread if it is joinable.
	 * If the thread is not joinable, this function does nothing.
	 */
    void join()
    {
    	if (_thread != NULL)
    	{
    		if (_thread->joinable())
    		{
    			_thread->join();
    		}
    	}
    };

	/**
	 * Detaches the thread object from the main thread.
	 *
	 * This function will detach the thread object from the main thread if it is joinable.
	 * If the thread is not joinable, this function does nothing.
	 */
    void detach()
    {
    	if (_thread != NULL)
    	{
    		_thread->detach();
    	}
    };

	/**
	 * @brief Stop the thread.
	 *
	 * @details Causes the thread to stop.  
	 */
    virtual void stop() 
	{ 
		// Cooperatively tell the thread to stop by setting the flag false- we're
		// no longer running.  This is VIRTUAL so that you can add additional 
		// stop conditions on the derived class like closing sockets, etc. so
		// that if you're in a blocked state (Something LIKELY...) you can stop
		// the silly thing by yanking the carpet out from underneath the loop to
		// actually see this change in real or near real-time and properly bail.
		_run = false; 
	};

	/**
	 * Starts the thread object.
	 *
	 * This function will start a new thread if one does not already exist.  If a thread
	 * already exists, it will stop the thread, join it, and delete it before starting a
	 * new one.  If an exception is thrown during the cleanup or startup process,
	 * catch it and print the error message.
	 */
    void start()
    {
    	try
    	{
    		// Discard the thread, if any, and start a new one...
    		if (_thread != NULL)
    		{
    	    	try
    	    	{
					stop();
					join();
					delete _thread;
    	    	}
    	    	catch(std::exception& e)
    	    	{
    	    		printf("Runable : %s\n", e.what());
    	    	}
    		}
    		_thread = new thread(&Runable::runThread, this);
    	}
    	catch (std::exception& e)
    	{
    		printf("Runable : %s\n", e.what());
    	}
    };

    /**
     * sleep()
     *
     * This method pauses the execution of the current thread for a specified duration.
     * The duration is provided in milliseconds. It provides a cross-platform way to
     * suspend thread execution, similar to Java's Thread.sleep().
     *
     * @param msDuration The duration in milliseconds for which the thread will be paused.
     */
    void sleep(int msDuration)
    {
    	sleep_for(milliseconds(msDuration));
    };

    /**
     * yield()
     *
     * This method causes the calling thread to yield execution to
     * another thread.  While this is not part of the C++11 standard,
     * it is common in C code, etc. and is offered more in a best
     * practice manner.  If you're in a loop, you need to either block
     * or yield so that cycles are released back to the OS.
     */
    inline void yield(void)
    {
    	yield();
    };

    /**
     * runThread(void *arg)
     *
     * Internal helper function for running the thread by calling the user's
     * run() method.  This is used by the start() method to create the thread
     * and kick off the user's run() method.
     *
     * @param arg The Runable object associated with this thread.
     *
     * @note This function is private and should not be called by the user.
     */
    static void runThread(void *arg)
    {
        // Sidestep a screwball problem with some implementations of the C++11 standard
        // threading interfaces...
    	((Runable *)arg)->_run = true;
    	((Runable *)arg)->run();
    	((Runable *)arg)->_run = false;
    };

    bool isRunning (void ) { return _run; };

protected:
    thread *		_thread;
    atomic<bool> 	_run;

    /*
     * Main Loop of the class.  You should override this method to
     * provide your own implementation of the thread loop in child classes.
    */
    virtual void run(void) = 0;		/* We **NEVER** want someone trying to instantiate this base class */

};

/*
 * This is, sort-of, a "simplification" of the Runable class.  It possesses
 * many of the same requirements as a Runable, but unlike a Runable,
 * it's dynamic allocation only, and once ::start() is ran, the ownership
 * of the memory allocation associated with the parent and any children
 * automatically becomes the OneShot's as it *self-destructs* on completion
 * of the thread.
 *
 * This means:
 *
 * - The associated thread is *completely* detached from the parent.
 * - You can't safely refer to the pointer except inside the run() method
 *   definition if you're making a self-runner, etc.
 *
 * As a result, one should avoid using a forever loop in the run()
 * redefinition unless it actually absolutely makes sense (most of the
 * time this won't do so- use Runable instead!)- you'll have a detached
 * thread with mostly NO control over it unless you've got some messaging
 * in place to give it the hint that it needs to die.
 *
 * Why would you *ever* want this?  Because you have something where you
 * stage up worker threads but don't want/need to reap them- you want
 * stage up and forget.  A good example of this would be something like
 * stage up blink engine, where you want to do a simple pattern and
 * then quit after the completion of the pattern- but you don't want
 * to tie any of the other LED management functions when you do it.
 * OneShot makes for simple worker threading that you can stack up
 * dozens of them and have them run in parallel and then they clean
 * themselves up after they're done.
 *
 * It's a design pattern that would be occasionally needed- but should
 * be used fairly sparingly...while it's a solid solution for a small
 * set of problems, it's not exactly what one would call safe for
 * larger use for obvious reasons.  If you're not in the proper context
 * here, USE Runable INSTEAD.  This is to keep you from making immortal
 * threads that you can't kill easily.
 *
 */
class OneShot : public NONCOPY
{
public:
    /// Default constructor
	OneShot() : _thread(NULL) {} ;

    /**
     * @brief Starts the OneShot thread.  If a thread already exists,
     * it is deleted and a new thread is started in its place.  The
     * thread is then arbitrarily detached.
     *
     * Note: If you call start() on an instance of this, it no longer
     * belongs to *ANYONE* except itself- this SELF-DESTRUCTS!
     *
     * @exception std::exception Thrown if there is an error starting the thread.
     */
    void start()
    {
    	try
    	{
    		// Discard the thread, if any (not likely- OneShot...), and start a new one,
    		// followed by arbitrarily detaching the same...
        	if (_thread != NULL)
        	{
        		try
        		{
            		delete _thread;
        		}
            	catch (std::exception& e)
            	{
            		printf("OneShot : %s\n", e.what());
            	}
        	}
        	_thread = new thread(&OneShot::runThread, this);
    		_thread->detach();
    	}
    	catch (std::exception& e)
    	{
    		printf("OneShot : %s\n", e.what());
    	}
    }

    /**
     * runThread(void *arg)
     *
     * Internal helper function for running the thread by calling the user's
     * run() method.  This is used by the start() method to create the thread
     * and kick off the user's run() method.
     *
     * @param arg The Runable object associated with this thread.
     *
     */
    static void runThread(void *arg) { ((OneShot *)arg)->run(); delete((OneShot *) arg); };

protected:

    /*
     * Main Loop of the class.  You should override this method to
     * provide your own implementation of the thread loop in child classes.
    */
    virtual void run(void) = 0;		// We **NEVER** want someone trying to instantiate this class

/**
 * @brief Destructor for the OneShot class.
 */
    virtual ~OneShot() 			    // We **NEVER** want an on-stack or global instance of this base or derivative.
    {
    	// However, we *DO* want it to clean up after itself...
    	if (_thread != NULL)
    	{
    		delete _thread;
    	}
    }

private:
    thread *_thread;	// Unlike Runable, we don't want the ability for children to see this.

};





