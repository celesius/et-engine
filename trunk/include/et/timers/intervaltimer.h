#pragma once

#include <et/core/tools.h>

namespace et
{
	class IntervalTimer
	{
	public:
		IntervalTimer(bool runNow) : _startTime(0.0f), _endTime(0.0f) 
			{ if (runNow) run(); }

		void run() 
		{ 
			_runTime = queryTime();
			_startTime = _runTime; 
		}

		float lap()
		{
			_endTime = queryTime();
			float dt = _endTime - _startTime;
			_startTime = _endTime;
			return dt;
		}

		float duration()
			{ return queryTime() - _runTime; }

	private:
		float _runTime;
		float _startTime;
		float _endTime;
	};
}