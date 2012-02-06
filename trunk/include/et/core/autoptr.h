#pragma once

namespace et
{
	template <typename T>
	class AutoPtr
	{
	public: 
		AutoPtr() : _data(0) 
			{ }

		explicit AutoPtr(T* d) : _data(d)
			{ } 

		AutoPtr(AutoPtr<T>& r) : _data(r.extract())
			{ }

		~AutoPtr()
		{ 
			delete _data;
			_data = 0;
		}

		inline char* binary()
			{ return reinterpret_cast<char*>(_data); }

		inline const char* binary() const
			{ return reinterpret_cast<const char*>(_data); }

		inline T* ptr()
			{ return _data; }

		inline const T* ptr() const 
			{ return _data; }

		inline T* extract()
		{
			T* value = _data;
			_data = 0;
			return value;
		}

		inline T* operator -> ()
			{ return _data; }

		inline const T* operator -> () const
			{ return _data; }

		T* operator = (T* r) 
			{ return reset(r); }

		inline T* reset(T* r)
		{
			if (r == _data) return _data;

			delete _data;
			_data = r;
			return _data;
		}

		inline bool isNull() const
			{ return _data == 0; }

		inline bool valid() const 
			{ return _data != 0; }

	private:
		T* _data;
	};

}