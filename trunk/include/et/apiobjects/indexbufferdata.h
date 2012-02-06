#pragma once

#include <et/apiobjects/apiobject.h>
#include <et/vertexbuffer/indexarray.h>

namespace et
{
	class RenderState;
	class IndexBufferData : public APIObjectData
	{
	public:
		IndexBufferData(RenderState& rs, IndexArrayRef i, BufferDrawType drawType, const std::string& name = std::string());
		~IndexBufferData();

		GLenum geometryType() const
			{ return _geometryType; }
		
		GLenum dataType() const
			{ return _dataType; }

		void* indexOffset(size_t offset) const;

		GLuint indexBuffer() const
			{ return _indexBuffer; }

		size_t size() const
			{ return _size; }

		void setSourceTag(size_t tag)
			{_sourceTag = tag; }

		size_t sourceTag() const
			{ return _sourceTag; }

		void setData(const IndexArrayRef& i);

	private:
		void setProperties(const IndexArrayRef& i);
		void build(const IndexArrayRef& i);

	private:
		RenderState& _rs;
		size_t _size;
		size_t _sourceTag;
		GLuint _indexBuffer;
		GLenum _dataType;
		GLenum _geometryType;
		BufferDrawType _drawType;
	};
}
