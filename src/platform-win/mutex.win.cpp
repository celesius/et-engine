/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <Windows.h>
#include <et/threading/mutex.h>

namespace et
{
	class MutexPrivate
	{
	public:
		MutexPrivate()
			{ _mutex = CreateMutex(0, false, 0); }

		~MutexPrivate()
			{ CloseHandle(_mutex); }

		void lock()
			{ WaitForSingleObject(_mutex, INFINITE); }

		void unlock()
			{ ReleaseMutex(_mutex); }

	private:
		HANDLE _mutex;
	};
}

using namespace et;

Mutex::Mutex() : _private(new MutexPrivate)
{
}

Mutex::~Mutex()
{
	delete _private;
}

void Mutex::lock()
{
	_private->lock();
}

void Mutex::unlock()
{
	_private->unlock();
}