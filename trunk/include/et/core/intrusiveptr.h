/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/threading/atomiccounter.h>

namespace et
{
	#define ET_DECLARE_POINTER(T)	typedef et::IntrusivePtr<T> Pointer;
	
	class Shared : virtual public AtomicCounter
	{
	public:   
		Shared()
			{ }

	private:
		ET_DENY_COPY(Shared)
	};

	template <typename T>
	class IntrusivePtr
	{
	public:
		IntrusivePtr() : _data(nullptr)
			{ }

		IntrusivePtr(const IntrusivePtr& r) : IntrusivePtr()
			{ reset(r._data);  }

		IntrusivePtr(IntrusivePtr&& r) : IntrusivePtr()
			{ std::swap(_data, r._data); }
		
		template <typename R>
		IntrusivePtr(IntrusivePtr<R> r) : IntrusivePtr()
			{ reset(static_cast<T*>(r.ptr()));  }

		explicit IntrusivePtr(T* data) : IntrusivePtr()
			{ reset(data); }

		virtual ~IntrusivePtr() 
			{ reset(nullptr); }

		T* operator *() 
			{ return _data; }

		const T* operator *() const 
			{ return _data; }

		T* operator -> ()
			{ return _data; }

		const T* operator -> () const 
			{ return _data; }

		T* ptr()
			{ return _data; }

		const T* ptr() const 
			{ return _data; }

		T& reference()
			{ return *_data; }

		const T& reference() const
			{ return *_data; }

		bool invalid() const
			{ return _data == nullptr; }

		bool valid() const 
			{ return _data != nullptr; }

		bool operator == (const IntrusivePtr& r) const
			{ return _data == r._data; }

		bool operator == (T* tr) const
			{ return _data == tr; }

		bool operator != (const IntrusivePtr& tr) const
			{ return _data != tr._data; }

		bool operator < (const IntrusivePtr& tr) const
			{ return _data < tr._data; }
		
		AtomicCounterType referenceCount() const
			{ return _data ? _data->atomicCounterValue() : 0; }

		IntrusivePtr<T>& operator = (const IntrusivePtr<T>& r)
		{ 
			reset(r._data); 
			return *this; 
		}

		template <typename R>
		IntrusivePtr<T>& operator = (IntrusivePtr<R> r)
		{
			reset(static_cast<T*>(r.ptr()));
			return *this;
		}

		void reset(T* data) 
		{
			if (data == _data) return;

			if (_data && (_data->release() == 0))
				delete _data;
			
			_data = data;

			if (_data)
				_data->retain();
		}

	private:
		T* _data;
	};

}