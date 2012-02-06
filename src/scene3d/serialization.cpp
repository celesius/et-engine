#include <et/scene3d/serialization.h>

namespace et
{
	namespace s3d
	{
		const SceneVersion SceneCurrentVersion = SceneVersion_1_0_0;
		const StorageVersion StorageCurrentVersion = StorageVersion_1_0_0;

		ChunkId HeaderScene = "ETSCN";
		ChunkId HeaderData = "SDATA";
		ChunkId HeaderElements = "ELMTS";
		ChunkId HeaderMaterials = "MTRLS";
		ChunkId HeaderVertexArrays = "VARRS";
		ChunkId HeaderIndexArrays = "IARRS";

		bool chunkEqualTo(ChunkId chunk, ChunkId comp)
		{
			size_t offset = 1;
			while (*chunk && (*chunk++ == *comp++)) ++offset;
			return offset == SerializationChunkLength;
		}
	}
}