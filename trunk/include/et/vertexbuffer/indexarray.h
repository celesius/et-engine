/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <vector>
#include <et/core/intrusiveptr.h>
#include <et/core/containers.h>

namespace et
{
	typedef unsigned int IndexType;
	typedef unsigned short ShortIndexType;
	typedef unsigned char SmallIndexType;
	
	static const IndexType InvalidIndex = static_cast<IndexType>(-1);	
	enum IndexArrayFormat
	{
		IndexArrayFormat_8bit = 1,
		IndexArrayFormat_16bit = 2,
		IndexArrayFormat_32bit = 4,
		IndexArrayFormat_max
	};

	enum IndexArrayContentType
	{
		IndexArrayContentType_Points,
		IndexArrayContentType_Lines,
		IndexArrayContentType_Triangles,
		IndexArrayContentType_TriangleStrips,
		IndexArrayContentType_max
	};

	class IndexArray : public Shared
	{
	public:
		typedef IntrusivePtr<IndexArray> Pointer;
		
		static const IndexType MaxShortIndex;
		static const IndexType MaxSmallIndex;

		int tag;

	public:
		IndexArray(IndexArrayFormat format, size_t size, IndexArrayContentType content);
		void linearize();

		const unsigned char* data() const 
			{ return _data.data(); }

		size_t dataSize() const
			{ return _data.dataSize(); }

		size_t capacity() const
			{ return _data.dataSize() / _format; }

		IndexArrayFormat format() const
			{ return _format; }

		IndexArrayContentType contentType() const
			{ return _contentType; }

		size_t actualSize() const
			{ return _actualSize; }

		void setActualSize(size_t value)
			{ _actualSize = value; }

		size_t primitivesCount() const;

		IndexType getIndex(size_t pos) const;
		void setIndex(IndexType value, size_t pos);
		void push_back(IndexType value);
		void resizeToFit(size_t count);
		void compact();

		void serialize(std::ostream& stream);
		void deserialize(std::istream& stream);

		struct Primitive
		{
			static const size_t MaxVertexCount = 3;
			IndexType index[MaxVertexCount];

			IndexType& operator [] (size_t i) 
				{ return index[i]; }

			const IndexType& operator [] (size_t i) const
				{ return index[i]; }

			Primitive();
			bool operator == (const Primitive& p) const;
			bool operator != (const Primitive& p) const;
		};

		class PrimitiveIterator
		{
		public:
			PrimitiveIterator& operator ++();
			PrimitiveIterator operator ++(int);

			bool operator == (const PrimitiveIterator& p) const;
			bool operator != (const PrimitiveIterator& p) const;

			const Primitive& operator *() const
				{ return _primitive; }
			Primitive& operator *()
				{ return _primitive; }

			const Primitive& operator ->() const
				{ return _primitive; }
			Primitive& operator ->()
				{ return _primitive; }

			size_t pos() const
				{ return _pos; }

		private:
			friend class IndexArray;

			PrimitiveIterator(const IndexArray* ib, size_t p);
			void configure(size_t p);

			PrimitiveIterator& operator = (const PrimitiveIterator& p);

		private:
			const IndexArray* _ib;
			Primitive _primitive;
			size_t _pos;
		};

		PrimitiveIterator begin() const;
		PrimitiveIterator end() const;
		PrimitiveIterator primitive(size_t index) const;

	private:
		BinaryDataStorage _data;
		size_t _actualSize;
		IndexArrayFormat _format;
		IndexArrayContentType _contentType;
	};

	typedef std::vector<IndexArray::Pointer> IndexArrayList;
}
