#include <et/core/serialization.h>
#include <et/vertexbuffer/vertexarray.h>

using namespace et;

const int VertexArrayId_1 = 'VAV1';
const int VertexArrayCurrentId = VertexArrayId_1;

VertexArray::VertexArray() : tag(0), _decl(true), _size(0), _smoothing(Usage_Smoothing, Type_Int, 0)
{
	
}

VertexArray::VertexArray(const VertexDeclaration& decl, size_t size) : tag(0), _decl(decl.interleaved()), _size(size), 
	_smoothing(Usage_Smoothing, Type_Int, size)
{
	for (size_t i = 0; i < decl.numElements(); ++i)
	{
		const VertexElement& e = decl.element(i);
		_decl.push_back(e.usage, e.type);
		_chunks.push_back(VertexDataChunk(e.usage, e.type, size));
	}
}

VertexArray::Description VertexArray::getDesc() const
{
	size_t dataSize = 0;
	size_t offset = 0;

	Description desc;
	desc.declaration = VertexDeclaration(_decl.interleaved());

	for (VertexDataChunkList::const_iterator i = _chunks.begin(), e = _chunks.end(); i != e; ++i)
	{
		VertexDataChunk chunk = *i;
		VertexElement entry;
		entry.usage = chunk->usage();
		entry.type = chunk->type();
		entry.stride = _decl.interleaved() ? static_cast<int>(_decl.dataSize()) : 0;
		entry.offset = _decl.interleaved() ? offset : static_cast<int>(dataSize);
		desc.declaration.push_back(entry);
		dataSize += chunk->dataSize();
		offset += chunk->typeSize();
	}

	desc.data.resize(dataSize);
	size_t numElements = dataSize / desc.declaration.dataSize();
	char* ptr0 = desc.data.binary();

	size_t entry_i = 0;
	for (VertexDataChunkList::const_iterator i = _chunks.begin(), e = _chunks.end(); i != e; ++i, ++entry_i)
	{
		VertexDataChunk chunk = *i;
		size_t chunkOffset = desc.declaration[entry_i].offset;
		if (desc.declaration.interleaved())
		{
			for (size_t j = 0; j < numElements; ++j)
			{
				char* dstPtr = ptr0 + chunkOffset + j * offset;
				const char* srcPtr = chunk->data() + j * chunk->typeSize();
				memcpy(dstPtr, srcPtr, chunk->typeSize());
			}
		}
		else
		{
			size_t size = chunk->dataSize();
			memcpy(ptr0 + chunkOffset, chunk->data(), size);
		}
	} 

	return desc;
}

VertexDataChunk VertexArray::chunk(VertexAttributeUsage usage)
{
	for (VertexDataChunkList::iterator i = _chunks.begin(), e = _chunks.end(); i != e; ++i)
	{
		if (i->ptr()->usage() == usage)
			return *i;
	}

	return VertexDataChunk();
}

void VertexArray::increase(size_t count)
{
	_size += count;
	for (VertexDataChunkList::iterator i = _chunks.begin(), e = _chunks.end(); i != e; ++i)
		(*i)->fitToSize(_size);

	_smoothing->fitToSize(_size);
}

void VertexArray::fitToSize(size_t count)
{
	if (_size >= count) return;

	_size = count;
	for (VertexDataChunkList::iterator i = _chunks.begin(), e = _chunks.end(); i != e; ++i)
		(*i)->fitToSize(count);

	_smoothing->fitToSize(_size);
}

void VertexArray::serialize(std::ostream& stream)
{
	serializeInt(stream, VertexArrayCurrentId);
	_decl.serialize(stream);
	serializeInt(stream, static_cast<int>(_size));
	serializeInt(stream, static_cast<int>(_chunks.size()));
	for (VertexDataChunkList::iterator i = _chunks.begin(), e = _chunks.end(); i != e; ++i)
		(*i)->serialize(stream);
	_smoothing->serialize(stream);
}

void VertexArray::deserialize(std::istream& stream)
{
	int id = deserializeInt(stream);
	if (id == VertexArrayId_1)
	{
		_decl.deserialize(stream);
		_size = deserializeInt(stream);
		size_t numChunks = deserializeInt(stream);
		for (size_t i = 0; i < numChunks; ++i)
			_chunks.push_back(VertexDataChunk(stream));
		_smoothing = VertexDataChunk(stream);
	}
}