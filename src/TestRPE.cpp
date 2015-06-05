//#define USE_TINYTHREAD

#include <Runnable.hpp>
#include <Singleton.hpp>
#include <stdio.h>
#include <poll.h>
#include <unistd.h>


// Keep it simple here.  Declare a simple singleton, and then
// declare a thread class that uses the singleton and does
// a few other things.

class SingletonTest : public Singleton<SingletonTest>
{
public:
	SingletonTest() {_count = 0;};
	void printAccessCount(void)
	{
		_lock.lock();
		printf("Singleton Count : %d\n\n", ++_count);
		_lock.unlock();
	};

	int getAccessCount(void)
	{
		return _count;
	}

private:
	mutex _lock;
	int _count;
};

class ThreadTest : public Runnable
{
public:
	ThreadTest(int stride) { _stride = stride; };

protected:
	virtual void run(void)
	{
		SingletonTest::GetInstance()->printAccessCount();
		for(int i = 0; i < 20; i += _stride)
		{
			printf("Thread %d - %d\n", _stride, i);
			this->sleep(_stride * 100);
		};
	}

private:
	int _stride;
};

class OneShotTest : public OneShot
{
public:
	OneShotTest()
	{
		SingletonTest::GetInstance()->printAccessCount();
		_stride = SingletonTest::GetInstance()->getAccessCount();
	};

protected:
	virtual void run(void)
	{
		OneShotTest *obj;

		printf("OneShot %d -- Start\n",_stride);

		for(int i = 0; i < 10; i += _stride)
		{
			printf("OneShot %d - %d\n", _stride, i);

			// For this test, since we don't have sleep() right at the moment
			// in OneShot, we're going to just cheat and use ::poll()
			::poll(NULL, 0, _stride * 100);
		};

		// Here's a cutiepie...allocate another
		// OneShotTest off the heap to 10 on stride.
		if (SingletonTest::GetInstance()->getAccessCount() < 10)
		{
			obj = new OneShotTest();
			obj->start();
		}

		::poll(NULL, 0, _stride * 100);
		printf("OneShot %d -- Dying\n", _stride);
	}

private:
	int _stride;
};

int main (int argc, char *argv[])
{
	// Do the OneShot tests...
	OneShotTest *obj = new OneShotTest();
	obj->start();

	// Do the thread/singleton tests...
	ThreadTest test1(1);
	ThreadTest test2(2);

	test1.start();
	test2.start();

	test2.join();

	sleep(10);

	return -1;
}
