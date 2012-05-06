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
		static const IndexType MaxShortIndex;
		static const IndexType MaxSmallIndex;

		int tag;

	public:
		IndexArray(IndexArrayFormat format, size_t size, IndexArrayContentType content);
		void linearize();

		IndexArrayFormat format() const;
		IndexArrayContentType contentType() const;
		size_t capacity() const;
		size_t dataSize() const;

		IndexType getIndex(size_t pos) const;
		void setIndex(IndexType value, size_t pos);
		void push_back(IndexType value);

		size_t actualSize() const;
		size_t primitivesCount() const;
		void setActualSize(size_t value);

		void resizeToFit(size_t count);
		void compact();

		const unsigned char* raw() const;
		void serialize(std::ostream& stream);
		void deserialize(std::istream& stream);

		struct Primitive
		{
			static const size_t MaxVertexCount = 3;
			IndexType index[MaxVertexCount];

			inline IndexType& operator [] (size_t i) 
				{ return index[i]; }

			inline const IndexType& operator [] (size_t i) const
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

	typedef IntrusivePtr<IndexArray> IndexArrayRef;
	typedef std::vector<IndexArrayRef> IndexArrayList;
}
