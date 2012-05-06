#include <iostream>
#include <et/threading/criticalsection.h>
#include <errno.h>
#include <pthread.h>

namespace et
{
	class CriticalSectionPrivate
	{
	public:
		pthread_mutex_t mutex;
	};
}

using namespace et;

CriticalSection::CriticalSection() : _private(new CriticalSectionPrivate)
{
	pthread_mutexattr_t attrib = { };
	pthread_mutexattr_init(&attrib);
	pthread_mutexattr_settype(&attrib, PTHREAD_MUTEX_RECURSIVE);
	
	pthread_mutex_init(&_private->mutex, &attrib);
	
	pthread_mutexattr_destroy(&attrib);
}

CriticalSection::~CriticalSection()
{
	pthread_mutex_destroy(&_private->mutex);
	delete _private;
}

void CriticalSection::enter()
{
	int result = pthread_mutex_lock(&_private->mutex);
	if (result == EBUSY)
		std::cout << "Mutex already locked." << std::endl;
}

void CriticalSection::leave()
{
	int result = pthread_mutex_unlock(&_private->mutex);
	if (result == EPERM)
		std::cout << "Mutex already unlocked or was locked from another thread." << std::endl;
}