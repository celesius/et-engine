/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <assert.h>
#include <et/platform/platform.h>

namespace et
{

	class Container
	{
	public:
		virtual ~Container() { }

		virtual void fill(int) = 0;

		virtual void* raw() = 0;
		virtual const void* raw() const = 0;

		virtual char* binary() = 0;
		virtual const char* binary() const = 0;

		virtual const size_t size() const = 0;
		virtual const size_t dataSize() const = 0;
		virtual const size_t typeSize() const = 0;
	};

	template <typename T, size_t count>
	struct StaticDataStorage : private Container
	{
		StaticDataStorage() 
			{ }
		
		StaticDataStorage(int initialize) 
			{ memset(data, initialize, sizeof(data)); }

		T data[count];

		void fill(int value)
			{ memset(data, value, dataSize()); }

		T& operator [](int i)
			{ return data[i]; }
		
		const T& operator [](int i) const
			{ return data[i]; }

		T* ptr()
			{ return data; }

		void* raw()
			{ return data; }
		
		const void* raw() const
			{ return data; };

		char* binary()
			{ return (char*)data; }
		
		const char* binary() const
			{ return (char*)data; }

		const size_t size() const
			{ return count; }
		
		const size_t dataSize() const 
			{ return count * sizeof(T); }
		
		const size_t typeSize() const
			{ return sizeof(T); }
	};

	template <typename T>
	class DataStorage : private Container
	{
	public:
		typedef T DataFormat;

		DataStorage() : _data(0), _size(0), _dataSize(0), _index(0)
		{ }

		explicit DataStorage(size_t size) : _data(0), _size(0), _dataSize(0), _index(0) 
		{ 
			resize(size); 
		}

		DataStorage(size_t size, int initValue) : _data(0), _size(0), _dataSize(0), _index(0) 
		{ 
			resize(size); 
			fill(initValue);
		}

		DataStorage(const DataStorage& copy) : _data(0), _size(0), _dataSize(0), _index(0) 
		{
			resize(copy.size());
			memcpy(_data, copy.data(), copy.dataSize());
		}

		~DataStorage()
		{ 
			resize(0); 
		}

		void resize(size_t size)
		{
			if (_size == size) return;

			T* new_data = 0;
			size_t min_size = (size < _size) ? size : _size;
			_size = size;
			_dataSize = _size * sizeof(T);

			if (size > 0)
			{
				new_data = new T[size];
				if (min_size > 0)
					memcpy(new_data, _data, min_size * sizeof(T));
			}
			else
			{
				_index = 0;
			}

			delete _data;

			_data = new_data;
		}

		void fill(int value) 
			{ memset(_data, value, _dataSize); }

		void operator ++()
			{ ++_index; }
		
		T& current()
			{ return _data[_index]; }
		
		T* current_ptr()
			{ return &_data[_index]; }
		
		const size_t currentIndex() const
			{ return _index; }

		const size_t size() const
			{ return _size; }
		
		const size_t dataSize() const
			{ return _dataSize; }
		
		const size_t typeSize() const
			{ return sizeof(T); }

		T* data()
			{ return _data; }
		
		const T* data() const
			{ return _data; }

		T* extract()
		{
			T* value = _data;
			_data = 0;
			_size = 0;
			_dataSize = 0;
			_index = 0;
			return value;
		}

		T* element_ptr(size_t i)
			{ return &_data[i]; }

		const T* element_ptr(size_t i) const 
			{ return &_data[i]; }

		T& operator [](size_t i)
			{ assert(i < _size); return _data[i]; }

		const T& operator [](size_t i) const 
			{ assert(i < _size); return _data[i]; } 

		void* raw() 
			{ return static_cast<void*>(_data); }

		const void* raw() const
			{ return static_cast<const void*>(_data); }

		char* binary()
			{ return (char*)(_data); }

		const char* binary() const
			{ return (const char*)(_data); }

		void fitToSize(size_t size)
		{
			size_t need_size = _index + size;
			if (need_size > _size)
				resize(need_size);
		}

		void push_back(const T& value)
		{
			fitToSize(1);
			_data[_index] = value;
			++_index;
		}

		void offset(size_t o) 
			{ _index += o; }

		void setOffset(size_t o) 
			{ _index = o; }

		DataStorage& operator = (const DataStorage& buf)
		{
			_index = buf._index;
			resize(buf.size());
			memcpy(_data, buf.data(), buf.dataSize());
			return *this;
		}

	private:
		T* _data;
		size_t _size;
		size_t _dataSize;
		size_t _index;
	};

	typedef DataStorage<float> FloatDataStorage;
	typedef DataStorage<unsigned char> BinaryDataStorage;
	typedef DataStorage<char> StringDataStorage;

	template <typename T>
	class RawDataAcessor : public Container
	{
	public:
		static const int TypeSize = sizeof(T);

		RawDataAcessor() : _dataSize(0), _size(0), _stride(0), _offset(0), _data(0) { }

		RawDataAcessor(char* data, const size_t dataSize, const size_t stride, const size_t offset) : _data(data) 
		{ 
			size_t estimatedDataSize = dataSize - offset;

			_offset = offset;
			_stride = stride ? stride : TypeSize;
			_dataSize = (estimatedDataSize / _stride) * _stride;
			_size = _dataSize / _stride;
		}

		T& operator[] (size_t i) 
		{ 
			return *(reinterpret_cast<T*>(_data + i * _stride + _offset));
		}

		void fill(int v)
			{ memset(_data, v, _dataSize); }

		void* raw() { return _data; }
		const void* raw() const { return _data; }

		char* binary() { return (char*)_data; };
		const char* binary() const  { return (char*)_data; };

		const size_t size() const { return _size; };
		const size_t dataSize() const { return _dataSize; }
		const size_t typeSize() const { return TypeSize; }

		bool valid() const { return _data != 0; }

		RawDataAcessor& operator = (const RawDataAcessor& r)
		{
			_dataSize = r._dataSize;
			_size = r._size;
			_stride = r._stride;
			_offset = r._offset;
			_data = r._data;
		}

	private:
		size_t _dataSize;
		size_t _size;
		size_t _stride;
		size_t _offset;
		char* _data;
	};

}