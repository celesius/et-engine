/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/rendering/rendering.h>

namespace et
{
	struct VertexElement
	{
		VertexAttributeUsage usage;
		VertexAttributeType type; 
		int stride;
		size_t offset;

		VertexElement() : usage(Usage_Undefined), type(Type_Undefined), stride(0), offset(0) 
			{ }

		VertexElement(VertexAttributeUsage u, VertexAttributeType t, int stride_ = 0, size_t offset_ = 0) : 
			usage(u), type(t), stride(stride_), offset(offset_) { }

		bool operator == (const VertexElement& r) const
			{ return (usage == r.usage) && (type == r.type) && (stride == r.stride) && (offset == r.offset); }

		bool operator != (const VertexElement& r) const
			{ return (usage != r.usage) || (type != r.type) || (stride != r.stride) || (offset != r.offset); }
	};

	typedef std::vector<VertexElement> VertexElementList;
	typedef VertexElementList::iterator VertexElementListIterator;

	class VertexDeclaration
	{
	public:
		VertexDeclaration();
		VertexDeclaration(bool interleaved);
		VertexDeclaration(bool interleaved, VertexAttributeUsage usage, VertexAttributeType type);

		bool has(VertexAttributeUsage usage) const;

		bool push_back(VertexAttributeUsage usage, VertexAttributeType type);
		bool push_back(const VertexElement& element);

		bool remove(VertexAttributeUsage usage);
		void clear();

		const VertexElement& element(size_t i) const;

		const VertexElementList& elements() const
			{ return _list; }   

		VertexElement& operator [](size_t i)
			{ return _list.at(i); }

		size_t numElements() const
			{ return _list.size(); }

		size_t dataSize() const
			{ return _totalSize; }

		bool interleaved() const
			{ return _interleaved; }

		bool operator == (const VertexDeclaration& r) const;
		bool operator != (const VertexDeclaration& r) const
			{ return !(operator == (r)); }

		void serialize(std::ostream& stream);
		void deserialize(std::istream& stream);

	private:  
		static VertexElement _empty;
		VertexElementList _list;
		size_t _totalSize;
		bool _interleaved;
	};

}