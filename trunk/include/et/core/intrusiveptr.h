/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/threading/referencecounter.h>

namespace et
{
	class Shared : public ReferenceCounter
	{
	public:   
		Shared() 
			{ }

	private:
		Shared(const Shared&) 
			{ }

		Shared& operator = (const Shared&)
			{ return *this; }
	};

	template <typename T>
	class IntrusivePtr
	{
	public:
		IntrusivePtr() : _data(0)
			{ }

		IntrusivePtr(const IntrusivePtr& r) : _data(0)
			{ reset(r._data);  }

		template <typename R>
		IntrusivePtr(IntrusivePtr<R> r) : _data(0)
			{ reset(static_cast<T*>(r.ptr()));  }

		explicit IntrusivePtr(T* data) : _data(0)
			{ reset(data); }

		virtual ~IntrusivePtr() 
			{ reset(0); }

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

		bool operator != (const IntrusivePtr& r) const
			{ return _data != r._data; }

		ReferenceCounter referenceCount() const
			{ return _data ? _data->referenceCount() : 0; }

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