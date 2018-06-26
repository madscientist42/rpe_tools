/*
 * SysFSGPIO.h
 *
 *  Created on: Jun 25, 2018
 *      Author: frank
 */

#ifndef INCLUDE_SYSFSGPIO_H_
#define INCLUDE_SYSFSGPIO_H_

// This is LINUX-only...so while the toolset's cross-platform, until we can
// devise a similar solution set for Windows and OSX, we'll block this include
// which will get installed on other targets, if it's not Linux...
#if defined(__linux__)

#include <NONCOPY.hpp>
#include <Runnable.hpp>
#include <unistd.h>

#include <functional>
using std::function;

#include <string>
using std::string;

// Helpful typedefs...

//  Direction, used to help indicate IN/OUT for code.
typedef enum
{
	IN,
	OUT
} Direction;

// Value, used to help code specify signal HIGH/LOW...
typedef enum
{
	LOW,
	HIGH,
	INVALID
} Value;

// Edge, used to help code specify edge(s) for event callback...
typedef enum
{
	NONE,
	RISING,
	FALLING,
	BOTH
} Edge;

typedef function<void(Value)> CallbackFunction;


class SysFSGPIO: public Runnable
{
public:
	// In/Out constructor without an event callback for input...
	SysFSGPIO(uint16_t id, Direction direction);

	// In constructor with an event callback for input...  Destructor **MUST** unwind the
	// callback thread when it's this case...
	SysFSGPIO(uint16_t id, Edge edge, CallbackFunction callback);

	// Destructor...should correctly unwind unless something catastrophic happens...
	virtual ~SysFSGPIO();

	// Get my value...if a callback is set on us, we return INVALID.
	// (You're supposed to use the callback's return... )
	Value getValue(void);

	// Set my value...returns value or invalid...
	Value setValue(Value value);

	// Get my ID...
	uint16_t getID(void) { return _id; }

protected:
	virtual void run(void);

private:
	// The generic path into the sysfs GPIO class edge...
	static const string  _sysfsPath;

	// MY GPIO's internal params...
	uint16_t 				_id;			// What's the GPIO number for this object?
	string					_id_str;		// String version of the GPIO number...
	Direction   			_direction;		// What direction was set on init?
	Edge					_edge;			// What (optional) edge was set on init?
	CallbackFunction		_callback;		// Did we have a callback set on us?
	int						_fd;			// Is there an FD opened against this GPIO?

	// Export out GPIO...
	void exportGPIO(void);

	// Unexport the GPIO
	void unexportGPIO(void);

	// Quick, NASTY way to check if a path exists...
	bool PathExists( const std::string &Pathname ) { return access( Pathname.c_str(), 0 ) == 0;	};
};

#endif // #if defined(__linux__)

#endif /* INCLUDE_SYSFSGPIO_H_ */
