#if !defined(__NONCOPY_HPP)
#define __NONCOPY_HPP

/*
 * NONCOPY.hpp
 *
 *  Created on: 30.11.2019
 *      Author: Frank Earl
 *
 * Provides a base class that transofrms a class into a noncopyable class
 */

class NONCOPY
{
protected:
   NONCOPY()  = default;
   ~NONCOPY() = default;
private:
   NONCOPY(const NONCOPY&)            = delete;
   NONCOPY& operator=(const NONCOPY&) = delete;
   NONCOPY(NONCOPY&&)                 = delete;
   NONCOPY& operator=(NONCOPY&&)      = delete;
};

#endif

