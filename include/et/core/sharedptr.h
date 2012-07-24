/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/threading/ReferenceCounter.h>

namespace et
{
	template <typename T> class SharedPtr;
	template <typename T> class WeakPtr;

	struct SharedReferenceCounter
	{
		ReferenceCounter strong;
		ReferenceCounter weak;
	};

	template <typename T>
	class SharedPtr
	{
	public:
		SharedPtr() : _data(0), _refCount(0)
			{ };

		virtual ~SharedPtr()
			{ reset(0, 0); }

		explicit SharedPtr(T* p) : _data(0), _refCount(0) 
			{ reset(p, 0); };

		SharedPtr(const SharedPtr<T>& p) : _data(0), _refCount(0) 
			{ reset(p._data, p._refCount); };

		template <typename R>
		SharedPtr(SharedPtr<R> p) : _data(0), _refCount(0) 
			{ reset(p.ptr(), p.refCount()); };

		SharedPtr& operator = (const SharedPtr& r)
		{ 
			reset(r._data, r._refCount);
			return *this;
		}

		SharedPtr& operator = (T* d)
		{ 
			reset(d, 0);
			return *this;
		}

		bool valid() const
			{ return _data != 0; }

		T* ptr()
			{ return _data; }

		const T* ptr() const
			{ return _data; }

		bool invalid() const
			{ return !valid(); }

		T* operator -> () 
			{ return ptr(); }

		const T* operator -> () const
			{ return ptr(); }

		T* operator *() 
			{ return ptr(); }

		const T* operator *() const 
			{ return ptr(); }

		bool operator == (const SharedPtr& r) const
			{ return _data == r._data; }

		bool operator == (T* p) const
			{ return _data == p; }

		bool operator != (const SharedPtr& r) const
			{ return _data != r._data; }

		ReferenceCounterType referenceCount() const
			{ return _refCount ? _refCount->strong.referenceCount() : 0; }

		SharedReferenceCounter* refCount() 
			{ return _refCount; }

	private:

		void reset(T* data, SharedReferenceCounter* r)
		{
			if (data == _data) return;

			if (_refCount && _refCount->strong.release() == 0)
			{
				delete _data;
				if (_refCount->weak.referenceCount() == 0)
					delete _refCount;
			}

			_data = data;
			_refCount = r;

			if (_data && !_refCount)
				_refCount = new SharedReferenceCounter;

			if (_refCount)
				_refCount->strong.retain();
		}

	private:
		T* _data;
		SharedReferenceCounter* _refCount;

		friend class WeakPtr<T>;
	};

	template <typename T>
	class WeakPtr
	{
	public:
		WeakPtr() : _data(0), _refCount(0)
			{ };

		virtual ~WeakPtr()
			{ reset(0, 0); }

		explicit WeakPtr(T* p) : _data(0), _refCount(0) 
			{ reset(p, 0); };

		WeakPtr(const WeakPtr<T>& p) : _data(0), _refCount(0) 
			{ reset(p._data, p._refCount); };

		WeakPtr(const SharedPtr<T>& p) : _data(0), _refCount(0) 
			{ reset(p._data, p._refCount); };

		WeakPtr& operator = (const WeakPtr& r)
		{ 
			reset(r._data, r._refCount);
			return *this;
		}

		WeakPtr& operator = (const SharedPtr<T>& r)
		{ 
			reset(r._data, r._refCount);
			return *this;
		}

		WeakPtr& operator = (T* d)
		{ 
			reset(d, 0);
			return *this;
		}

		bool valid() const
			{ return _refCount && (_refCount->strong.referenceCount() > 0); }

		T* ptr()
			{ return valid() ? _data : 0; }

		const T* ptr() const
			{ return valid() ? _data : 0; }

		bool invalid() const
			{ return !valid(); }

		T* operator -> () 
			{ return ptr(); }

		const T* operator -> () const
			{ return ptr(); }

		T* operator *() 
			{ return ptr(); }

		const T* operator *() const 
			{ return ptr(); }

		bool operator == (const WeakPtr& r) const
			{ return _data == r._data; }

		bool operator != (const WeakPtr& r) const
			{ return _data != r._data; }

		ReferenceCounterType referenceCount() const
			{ return _refCount ? _refCount->strong.referenceCount() : 0; }

	private:

		void reset(T* data, SharedReferenceCounter* r)
		{
			if (data == _data) return;

			if (_refCount && (_refCount->weak.release() == 0))
				delete _refCount;

			_data = data;
			_refCount = r;

			if (_data && !_refCount)
				_refCount = new SharedReferenceCounter;

			if (_refCount)
				_refCount->weak.retain();
		}

	private:
		T* _data;
		SharedReferenceCounter* _refCount;
	};
}