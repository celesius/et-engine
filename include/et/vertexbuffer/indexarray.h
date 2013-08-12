/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/core/intrusiveptr.h>
#include <et/core/containers.h>
#include <et/opengl/opengltypes.h>

namespace et
{
	class IndexArray : public Shared
	{
	public:
		ET_DECLARE_POINTER(IndexArray)
		
		static const IndexType MaxShortIndex;
		static const IndexType MaxSmallIndex;

		int tag;

	public:
		IndexArray(IndexArrayFormat format, size_t size, PrimitiveType primitiveType);
		void linearize();

		const unsigned char* data() const 
			{ return _data.data(); }

		size_t dataSize() const
			{ return _data.dataSize(); }

		size_t capacity() const
			{ return _data.dataSize() / _format; }

		IndexArrayFormat format() const
			{ return _format; }

		PrimitiveType primitiveType() const
			{ return _primitiveType; }

		size_t actualSize() const
			{ return _actualSize; }

		void setActualSize(size_t value)
			{ _actualSize = value; }

		size_t primitivesCount() const;

		IndexType getIndex(size_t pos) const;
		void setIndex(IndexType value, size_t pos);
		void push_back(IndexType value);

		void resize(size_t count);
		void resizeToFit(size_t count);

		void compact();

		void serialize(std::ostream& stream);
		void deserialize(std::istream& stream);

		class Primitive
		{
		public:
			enum
			{
				VertexCount_max = 3
			};

			IndexType index[VertexCount_max];
			
		public:
			Primitive();
			
			IndexType& operator [] (size_t i)
				{ assert(i < VertexCount_max); return index[i]; }

			const IndexType& operator [] (size_t i) const
				{ assert(i < VertexCount_max); return index[i]; }

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
			
			IndexType& operator [] (size_t i)
				{ return _primitive[i]; }
			
			const IndexType& operator [] (size_t i) const
				{ return _primitive[i]; }

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
		PrimitiveType _primitiveType;
	};

	typedef std::vector<IndexArray::Pointer> IndexArrayList;
}
