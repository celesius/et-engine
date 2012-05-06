/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

namespace et
{
	typedef unsigned long ThreadResult;
	typedef unsigned long ThreadId;

	class ThreadPrivate;
	class Thread
	{
	public:
		Thread(bool start = true);
		virtual ~Thread();

		void run();
		void terminate(int result = 0);

		bool running() const;
		bool suspended() const;

		ThreadId id() const;
		virtual ThreadResult main();

	protected:
		void suspend();
		void resume();

	private:
		Thread(const Thread&)
			{ }

		Thread& operator = (const Thread&)
			{ return *this; }

	private:
		friend class ThreadPrivate;
		ThreadPrivate* _private;
	};
}