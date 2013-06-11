/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/core/containersbase.h>

namespace et
{
	template <typename T>
	class DataStorage : public ContainerBase<T>
	{
	public:
		typedef T DataFormat;

		DataStorage() : 
			_mutableData(0), _size(0), _dataSize(0), _offset(0),
			_flags(DataStorageFlag_OwnsMutableData) { }

		explicit DataStorage(size_t size) :
			_mutableData(0), _size(0), _dataSize(0), _offset(0),
			_flags(DataStorageFlag_OwnsMutableData)
		{
			resize(size); 
		}

		DataStorage(size_t size, int initValue) :
			_mutableData(0), _size(0), _dataSize(0), _offset(0),
			_flags(DataStorageFlag_OwnsMutableData)
		{
			resize(size); 
			fill(initValue);
		}

		DataStorage(const DataStorage& copy) :
			_mutableData(0), _size(0), _dataSize(0), _offset(0),
			_flags(DataStorageFlag_OwnsMutableData)
		{
			resize(copy.size());
			if (copy.size() > 0)
				etCopyMemory(_mutableData, copy.data(), copy.dataSize());
		}
		
		DataStorage(T* data, size_t dataSize) :
			_mutableData(data), _size(dataSize / sizeof(T)), _dataSize(dataSize), _offset(0),
			_flags(DataStorageFlag_Mutable)
		{
		}

		DataStorage(const T* data, size_t dataSize) :
			_immutableData(data), _size(dataSize / sizeof(T)), _dataSize(dataSize), _offset(0),
			_flags(0)
		{
		}
		
		~DataStorage()
			{ resize(0); }

	public:
		/*
		 * assignment
		 */
		DataStorage& operator = (const DataStorage& buf)
		{
			if (buf.ownsData())
			{
				_offset = buf._offset;
				_flags = buf._flags;
				resize(buf.size());
				if (buf.size() > 0)
				{
					etCopyMemory(_mutableData, buf.data(), buf.dataSize());
				}
			}
			else
			{
				_offset = 0;
				_mutableData = buf._mutableData;
				_dataSize = buf._dataSize;
				_flags = buf._flags;
				_size = buf._size;
			}
			return *this;
		}
		
	public:
		/*
		 * mutable accessors
		 */
		T* data()
			{ assert(mutableData()); return _mutableData; }
		
		char* binary()
			{ assert(mutableData()); return reinterpret_cast<char*>(_mutableData); }
		
		T& operator [](size_t aIndex)
			{ assert(mutableData() && (aIndex < _size)); return _mutableData[aIndex]; }
		
		T& current()
			{ assert(mutableData() && (_offset < _size)); return _mutableData[_offset]; }

		T* current_ptr()
			{ assert(mutableData() && (_offset < _size)); return &_mutableData[_offset]; }
		
		T* element_ptr(size_t aIndex)
			{ assert(aIndex < _size); return &_mutableData[aIndex]; }
		
		/*
		 * const accessors
		 */
		const T* data() const
			{ return _immutableData; }

		const char* binary() const
			{ return reinterpret_cast<const char*>(_immutableData); }

		const T& operator [](size_t i) const
			{ assert(i < _size); return _immutableData[i]; }
		
		const T& current() const
			{ assert(_offset < _size); return _immutableData[_offset]; }

		const T* current_ptr() const
			{ assert(_offset < _size); return &_immutableData[_offset]; }
		
		const T* element_ptr(size_t i) const
			{ assert(i < _size); return &_immutableData[i]; }
		
		const size_t size() const
			{ return _size; }
		
		const size_t dataSize() const
			{ return _dataSize; }
		
		/*
		 * wrappers
		 */
		char* mutableBinaryData()
			{ return binary(); }
		
		const char* constBinaryData() const
			{ return binary(); }
		
		/*
		 * modifiers
		 */
		void fill(int value)
			{ assert(mutableData()); etFillMemory(_mutableData, value, _dataSize); }
		
		void resize(size_t size)
		{
			if (_size == size) return;
			
			T* new_data = nullptr;
			size_t min_size = (size < _size) ? size : _size;
			_size = size;
			_dataSize = _size * sizeof(T);
			
			if (size > 0)
			{
				new_data = new T[size];
				if (min_size > 0)
					etCopyMemory(new_data, _immutableData, min_size * sizeof(T));
			}
			else
			{
				_offset = 0;
			}
			
			if (ownsData())
				delete [] _mutableData;
			
			_flags |= DataStorageFlag_Mutable;
			_mutableData = new_data;
		}
		
		void push_back(const T& value)
		{
			assert(mutableData());
			fitToSize(1);
			assert(_offset < _size);
			_mutableData[_offset++] = value;
		}
		
		void append(const T* values, size_t count)
		{
			assert(mutableData());
			
			size_t currentSize = _size;
			resize(_size + count);
			etCopyMemory(&_mutableData[currentSize], values, count * sizeof(T));
		}
		
		void appendData(void* ptr, size_t dataSize)
		{
			assert(mutableData());
			
			size_t currentSize = _size;
			size_t numElements = dataSize / sizeof(T) + ((dataSize % sizeof(T) > 0) ? 1 : 0);
			resize(_size + numElements);
			etCopyMemory(&_mutableData[currentSize], ptr, dataSize);
		}
		
		T* extract()
		{
			T* value = _mutableData;
			_mutableData = nullptr;
			_size = 0;
			_dataSize = 0;
			_offset = 0;
			return value;
		}

		void fitToSize(size_t size)
		{
			size_t need_size = _offset + size;
			if (need_size > _size)
				resize(need_size);
		}

		const size_t offset() const
			{ return _offset; }
		
		void applyOffset(size_t o)
			{  assert(mutableData()); _offset += o; }

		void setOffset(size_t o) 
			{ assert(mutableData()); _offset = o; }

	private:
		enum
		{
			DataStorageFlag_OwnsData = 0x01,
			DataStorageFlag_Mutable = 0x02,
			DataStorageFlag_OwnsMutableData = DataStorageFlag_OwnsData | DataStorageFlag_Mutable,
		};
		
		bool ownsData() const
			{ return (_flags & DataStorageFlag_OwnsData) == DataStorageFlag_OwnsData; }
		
		bool mutableData() const
			{ return (_flags & DataStorageFlag_Mutable) == DataStorageFlag_Mutable; }
		
	private:
		union
		{
			T* _mutableData;
			const T* _immutableData;
		};
		
	private:
		size_t _size;
		size_t _dataSize;
		size_t _offset;
		size_t _flags;
	};

	typedef DataStorage<float> FloatDataStorage;
	typedef DataStorage<unsigned char> BinaryDataStorage;
	typedef DataStorage<char> StringDataStorage;
}
