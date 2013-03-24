/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

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
			SceneVersion_1_0_1 = 101,
			SceneVersion_1_0_2 = 102,
		};

		enum StorageVersion
		{
			StorageVersion_1_0_0 = 100,
		};

		extern const SceneVersion SceneVersionLatest;
		extern const StorageVersion StorageVersionLatest;

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