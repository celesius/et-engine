#pragma once

#include <ostream>
#include <istream>

namespace et
{
	namespace s3d
	{
		enum SerializationParameters
		{
			SerializationChunkLength = 6 
		};

		enum SceneVersion
		{
			SceneVersion_1_0_0 = 100,
		};

		enum StorageVersion
		{
			StorageVersion_1_0_0 = 100,
		};

		extern const SceneVersion SceneCurrentVersion;
		extern const StorageVersion StorageCurrentVersion;

		typedef char ChunkId[SerializationChunkLength];
		extern ChunkId HeaderScene;
		extern ChunkId HeaderData;
		extern ChunkId HeaderElements;
		extern ChunkId HeaderMaterials;
		extern ChunkId HeaderVertexArrays;
		extern ChunkId HeaderIndexArrays;

		inline void serializeChunk(std::ostream& stream, ChunkId chunk)
			{ stream.write(chunk, SerializationChunkLength); }

		inline void deserializeChunk(std::istream& stream, ChunkId chunk)
			{ stream.read(chunk, SerializationChunkLength); }

		bool chunkEqualTo(ChunkId chunk, ChunkId comp);
	}
}