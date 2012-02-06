#pragma once

#include <et/core/containers.h>
#include <et/core/intrusiveptr.h>
#include <et/vertexbuffer/vertexdeclaration.h>

namespace et
{
	class VertexDataChunkData : public Shared
	{
	public:
		VertexDataChunkData(std::istream& stream);
		VertexDataChunkData(VertexAttributeUsage usage, VertexAttributeType type, size_t size);

		void fitToSize(size_t size);

		inline char* data()
			{ return _data.binary(); }

		inline const char* data() const
			{ return _data.binary(); }

		inline const size_t size() const
			{ return _data.size(); }

		inline const size_t dataSize() const
			{ return _data.dataSize(); }

		inline const size_t typeSize() const
			{ return vertexAttributeTypeSize(_type); }

		inline VertexAttributeUsage usage() const
			{ return _usage; }

		inline VertexAttributeType type() const
			{ return _type; }

		void serialize(std::ostream& stream);

	private:
		VertexAttributeUsage _usage;
		VertexAttributeType _type;
		DataStorage<char> _data;
	};

	class VertexDataChunk : public IntrusivePtr<VertexDataChunkData>
	{
	public:
		VertexDataChunk()
			{ }

		VertexDataChunk(std::istream& stream) : 
			IntrusivePtr<VertexDataChunkData>(new VertexDataChunkData(stream))
			{ }

		VertexDataChunk(VertexAttributeUsage usage, VertexAttributeType type, size_t size) : 
			IntrusivePtr<VertexDataChunkData>(new VertexDataChunkData(usage, type, size))
			{ }

		template <typename T>
		inline RawDataAcessor<T> accessData(size_t elementOffset) 
		{
			return valid() ? 
				RawDataAcessor<T>(ptr()->data(), ptr()->dataSize(), ptr()->typeSize(), elementOffset * ptr()->typeSize()) : 
				RawDataAcessor<T>(); 
		}
	};

	typedef std::vector<VertexDataChunk> VertexDataChunkList;
}