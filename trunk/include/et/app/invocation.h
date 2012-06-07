/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/autoptr.h>
#include <et/app/runloop.h>
#include <et/tasks/tasks.h>

namespace et
{
	class PureInvocationTarget
	{
	public:
		PureInvocationTarget() { }
		virtual ~PureInvocationTarget() { };

		virtual void invoke() = 0;
		virtual PureInvocationTarget* copy() = 0;
	};

	class PureInvocation
	{
	public:
		PureInvocation() : _target(0) { };
		virtual ~PureInvocation() { };

		virtual void invoke() = 0;
		virtual void invokeInMainRunLoop(float delay) = 0;

	protected:
		AutoPtr<PureInvocationTarget> _target;
	};
	
	class InvocationTask : public Task
	{
	public:
		InvocationTask(PureInvocationTarget* invocation);
		~InvocationTask();
		void execute();

	private:
		PureInvocationTarget* _invocation;
	};

	template <typename T>
	class InvocationTarget : public PureInvocationTarget
	{
	public:
		InvocationTarget(T* o, void(T::*m)()) : _object(o), _method(m) { }

		void invoke()
			{ (_object->*_method)(); }

		PureInvocationTarget* copy()
			{ return new InvocationTarget(_object, _method); }

	private:
		T* _object;
		void(T::*_method)();
	};

	template <typename T, typename A1>
	class Invocation1Target : public PureInvocationTarget
	{
	public:
		Invocation1Target(T* o, void(T::*m)(A1), A1 p1) : _object(o), _method(m), _param(p1) { }

		void invoke()
			{ (_object->*_method)(_param); }

		void setParameter(A1 p1)
			{ _param = p1; }

		PureInvocationTarget* copy() 
			{ return new Invocation1Target(_object, _method, _param); }

	private:
		Invocation1Target operator = (const Invocation1Target&) 
			{ return *this; }

	private:
		T* _object;
		void(T::*_method)(A1);
		A1 _param;
	};

	template <typename T, typename A1, typename A2>
	class Invocation2Target : public PureInvocationTarget
	{
	public:
		Invocation2Target(T* o, void(T::*m)(A1, A2), A1 p1, A2 p2) : 
			_object(o), _method(m), _p1(p1), _p2(p2) { }

		void invoke()
			{ (_object->*_method)(_p1, _p2); }

		void setParameters(A1 p1, A2 p2)
		{ 
			_p1 = p1; 
			_p2 = p2;
		}

		PureInvocationTarget* copy() 
			{ return new Invocation2Target(_object, _method, _p1, _p2); }

	private:
		Invocation2Target operator = (const Invocation2Target&) 
			{ return *this; }

	private:
		T* _object;
		void(T::*_method)(A1, A2);
		A1 _p1;
		A2 _p2;
	};

	class Invocation : public PureInvocation
	{
	public:
		void invoke();
		void invokeInMainRunLoop(float delay = 0.0f);
		void invokeInRunLoop(RunLoop& rl, float delay = 0.0f);

		template <typename T>
		void setTarget(T* o, void(T::*m)())
			{ _target = new InvocationTarget<T>(o, m); }

	private:
		AutoPtr<PureInvocationTarget> _target;
	};

	class Invocation1 : public PureInvocation
	{
	public:
		void invoke();
		void invokeInMainRunLoop(float delay = 0.0f);
		void invokeInRunLoop(RunLoop& rl, float delay = 0.0f);

		template <typename T, typename A1>
		void setTarget(T* o, void(T::*m)(A1), A1 param)
			{ _target = new Invocation1Target<T, A1>(o, m, param); }

		template <typename T, typename A1>
		void setParameter(A1 p)
			{ (static_cast<Invocation1Target<T, A1>*>(_target.ptr()))->setParameter(p); }
	};

	class Invocation2 : public PureInvocation
	{
	public:
		void invoke();
		void invokeInMainRunLoop(float delay = 0.0f);
		void invokeInRunLoop(RunLoop& rl, float delay = 0.0f);

		template <typename T, typename A1, typename A2>
		void setTarget(T* o, void(T::*m)(A1, A2), A1 p1, A2 p2)
			{ _target = new Invocation2Target<T, A1, A2>(o, m, p1, p2); }

		template <typename T, typename A1, typename A2>
		void setParameters(A1 p1, A2 p2)
			{ (static_cast<Invocation2Target<T, A1, A2>*>(_target.ptr()))->setParameters(p1, p2); }
	};
}