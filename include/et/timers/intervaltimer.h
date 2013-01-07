/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/tools.h>

namespace et
{
	class IntervalTimer
	{
	public:
		IntervalTimer(bool runNow) : _startTimeMSec(0), _endTimeMSec(0)
			{ if (runNow) run(); }

		void run() 
		{ 
			_runTimeMSec = queryTimeMSec();
			_startTimeMSec = _runTimeMSec;
		}

		float lap()
		{
			_endTimeMSec = queryTimeMSec();
			uint64_t dt = _endTimeMSec - _startTimeMSec;
			_startTimeMSec = _endTimeMSec;
			return static_cast<float>(dt) / 1000.0f;
		}

		float duration()
			{ return static_cast<float>(queryTimeMSec() - _runTimeMSec) / 1000.0f; }

	private:
		uint64_t _runTimeMSec;
		uint64_t _startTimeMSec;
		uint64_t _endTimeMSec;
	};
}