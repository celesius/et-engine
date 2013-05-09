/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <fstream>
#include <et/core/tools.h>
#include <et/core/stream.h>
#include <et/rendering/rendercontext.h>
#include <et/scene3d/scene3d.h>
#include <et/scene3d/serialization.h>

using namespace et;
using namespace s3d;

Scene3d::Scene3d(const std::string& name) : ElementContainer(name, 0), _externalFactory(0)
{
}

void Scene3d::serialize(std::ostream& stream, StorageFormat fmt, const std::string& basePath)
{
	if (stream.fail()) return;

	Element::List storages = childrenOfType(ElementType_Storage);

	serializeChunk(stream, HeaderScene);
	serializeInt(stream, SceneVersionLatest);

	serializeChunk(stream, HeaderData);
	serializeInt(stream, StorageVersionLatest);

	serializeInt(stream, fmt);

	serializeInt(stream, storages.size());
	ET_START_ITERATION(storages, Scene3dStorage::Pointer, s)
	{
		serializeChunk(stream, HeaderMaterials);
		serializeInt(stream, s->materials().size());
		if (fmt == StorageFormat_Binary)
		{
			ET_ITERATE(s->materials(), auto&, mi,
			{
				serializeInt(stream, reinterpret_cast<int>(mi.ptr()));
				mi->serialize(stream, fmt);
			})
		}
		else if (fmt == StorageFormat_HumanReadableMaterials)
		{
			std::map<std::string, int> materialsMap;
			ET_START_ITERATION(s->materials(), auto&, mi)
			{
				std::string mFile;
				std::string matName = mi->objectName();
				
				if (materialsMap.find(matName) == materialsMap.end())
				{
					mFile = basePath + matName + ".xml";
					materialsMap[matName] = 1;
				}
				else
				{
					mFile = basePath + matName + intToStr(materialsMap[matName]) + ".xml";
					materialsMap[matName] = materialsMap[matName] + 1;
				}

				serializeInt(stream, reinterpret_cast<int>(mi.ptr()));
				serializeString(stream, mFile);

				std::ofstream mStream(mFile.c_str());
				mi->serialize(mStream, fmt);
				mStream.close();
			}
			ET_END_ITERATION
		}
		else
		{
			assert("Invalid storage format specified." && 0);
		}

		serializeChunk(stream, HeaderVertexArrays);
		serializeInt(stream, s->vertexArrays().size());
		ET_ITERATE(s->vertexArrays(), auto&, vi,
		{
			serializeInt(stream, reinterpret_cast<int>(vi.ptr()));
			vi->serialize(stream);
		})

		IndexArray::Pointer ia = s->indexArray();

		serializeChunk(stream, HeaderIndexArrays);
		serializeInt(stream, 1);
		serializeInt(stream, reinterpret_cast<int>(ia.ptr()));
		ia->serialize(stream);
	}
	ET_END_ITERATION

	serializeChunk(stream, HeaderElements);
	ElementContainer::serialize(stream, SceneVersionLatest);
}

bool Scene3d::deserialize(std::istream& stream, RenderContext* rc, TextureCache& tc,
	CustomElementFactory* factory, const std::string& basePath)
{
	if (stream.fail()) 
    {
		log::error("Unable to deserialize scene from stream.");
        return false;
    }
    
	ChunkId readChunk = { };
	deserializeChunk(stream, readChunk);
	if (!chunkEqualTo(readChunk, HeaderScene)) 
	{
		log::error("Data not looks like proper ETM file.");
		return false;
	}

	_externalFactory = factory;

	size_t version = deserializeInt(stream);
	if (version > static_cast<size_t>(SceneVersionLatest))
	{
		log::error("Unsupported version of the ETM file.");
		return false;
	}

	volatile bool readCompleted = false;
	while (!readCompleted)
	{
		deserializeChunk(stream, readChunk);
		if (chunkEqualTo(readChunk, HeaderData))
		{
			size_t storageVersion = deserializeInt(stream);
			
			size_t numStorages = 0;
			StorageFormat format = StorageFormat_Binary;
			
			if (storageVersion == StorageVersion_1_0_0)
			{
				numStorages = deserializeInt(stream);
			}
			else if (storageVersion == StorageVersion_1_0_1)
			{
				format = static_cast<StorageFormat>(deserializeInt(stream));
				numStorages = deserializeInt(stream);
			}
			else
			{
				log::error("Unsupported version of binary storage the ETM file.");
				return false;
			}

			for (size_t i = 0; i < numStorages; ++i)
			{
				Scene3dStorage::Pointer ptr = deserializeStorage(stream, rc, tc, basePath, format);
				ptr->setParent(this);
			}

		}
		else if (chunkEqualTo(readChunk, HeaderElements))
		{
			ElementContainer::deserialize(stream, this, static_cast<SceneVersion>(version));
			readCompleted = true;
		}
	}

	_externalFactory = 0;
	return true;
}


Scene3dStorage::Pointer Scene3d::deserializeStorage(std::istream& stream, RenderContext* rc,
	TextureCache& tc, const std::string& basePath, StorageFormat fmt)
{
	Scene3dStorage::Pointer result(new Scene3dStorage("storage", 0));

	volatile bool materialsRead = false;
	volatile bool vertexArraysRead = false;
	volatile bool indexArrayRead = false;

	while (!(materialsRead && vertexArraysRead && indexArrayRead))
	{
		ChunkId readChunk = { };
		deserializeChunk(stream, readChunk);

		if (chunkEqualTo(readChunk, HeaderMaterials))
		{
			size_t numMaterials = deserializeInt(stream);

			if (fmt == StorageFormat_Binary)
			{
				for (size_t i = 0; i < numMaterials; ++i)
				{
					Material m;
					m->tag = deserializeInt(stream);
					m->deserialize(stream, rc, tc, basePath, StorageFormat_Binary);
					result->addMaterial(m);
				}
			}
			else if (fmt == StorageFormat_HumanReadableMaterials)
			{
				for (size_t i = 0; i < numMaterials; ++i)
				{
					Material m;
					m->tag = deserializeInt(stream);
					m->setOrigin(basePath + getFileName(deserializeString(stream)));

					InputStream mStream(m->origin(), StreamMode_Text);
					
					if (mStream.valid())
						m->deserialize(mStream.stream(), rc, tc, basePath, fmt);

					result->addMaterial(m);
				}
			}
			else
			{
				assert("Invalid storage format specified" && false);
			}
			materialsRead = true;
		}
		else if (chunkEqualTo(readChunk, HeaderVertexArrays))
		{
			size_t numVertexArrays = deserializeInt(stream);
			for (size_t i = 0; i < numVertexArrays; ++i)
			{
				VertexArray::Pointer va(new VertexArray());
				va->tag = deserializeInt(stream);
				va->deserialize(stream);
				result->addVertexArray(va);
			}
			vertexArraysRead = true;
		}
		else if (chunkEqualTo(readChunk, HeaderIndexArrays))
		{
			int num = deserializeInt(stream);
			assert(num == 1);
			(void)(num);
			result->indexArray()->tag = deserializeInt(stream);
			result->indexArray()->deserialize(stream);
			indexArrayRead = true;
		}

		if (stream.eof()) break;
	}

	buildAPIObjects(result, rc);
	return result;
}

void Scene3d::buildAPIObjects(Scene3dStorage::Pointer p, RenderContext* rc)
{
	IndexBuffer ib;
	VertexArrayList& vertexArrays = p->vertexArrays();
	ET_START_ITERATION(vertexArrays, auto&, i)
	{
		std::string vbName = "vb-" + intToStr(static_cast<size_t>(i->tag));
		std::string ibName = "ib-" + intToStr(static_cast<size_t>(p->indexArray()->tag));
		std::string vaoName = "vao-" + ibName + "-" + vbName;
		
		VertexArrayObject vao = rc->vertexBufferFactory().createVertexArrayObject(vaoName);
		VertexBuffer vb = rc->vertexBufferFactory().createVertexBuffer(vbName, i, BufferDrawType_Static);
		if (!ib.valid())
		{
			ib = rc->vertexBufferFactory().createIndexBuffer(ibName, p->indexArray(), BufferDrawType_Static);
			_indexBuffers.push_back(ib);
		}
		vao->setBuffers(vb, ib);

		_vaos.push_back(vao);
		_vertexBuffers.push_back(vb);
	}
	ET_END_ITERATION

	rc->renderState().resetBufferBindings();
}

void Scene3d::serialize(const std::string& filename, s3d::StorageFormat fmt)
{
	std::ofstream file(filename.c_str(), std::ios::binary | std::ios::out);
	serialize(file, fmt, getFilePath(filename));
}

bool Scene3d::deserialize(const std::string& filename, RenderContext* rc, TextureCache& tc,
	CustomElementFactory* factory)
{
	std::ifstream file(filename.c_str(), std::ios::binary | std::ios::in);
	bool success = deserialize(file, rc, tc, factory, getFilePath(filename));

	if (!success)
		log::error("Unable to load scene from file: %s", filename.c_str());

	return success;
}

Element::Pointer Scene3d::createElementOfType(size_t type, Element* parent)
{
	switch (type)
	{
	case ElementType_Container:
		return ElementContainer::Pointer(new ElementContainer(std::string(), parent));

	case ElementType_Mesh:
		return Mesh::Pointer(new Mesh(std::string(), parent));

	case ElementType_SupportMesh:
		return SupportMesh::Pointer(new SupportMesh(std::string(), parent));

	case ElementType_Storage:
		return Scene3dStorage::Pointer(new Scene3dStorage(std::string(), parent));

	case ElementType_Camera:
		return CameraElement::Pointer(new CameraElement(std::string(), parent));

	default:
		{
			if (_externalFactory)
				return _externalFactory->createElementOfType(type, parent);
			else
				return ElementContainer::Pointer(new ElementContainer(std::string(), parent));
		}
	}
}

Material Scene3d::materialWithId(int id)
{
	Element::List storages = childrenOfType(ElementType_Storage);
	for (auto si = storages.begin(), se = storages.end(); si != se; ++si)
	{
		Scene3dStorage* storage = static_cast<Scene3dStorage*>(si->ptr());
		ET_ITERATE(storage->materials(), auto&, data, if (data->tag == id) return data)
	}
	
	return Material();
}

VertexArrayObject Scene3d::vaoWithIdentifiers(const std::string& vbid, const std::string& ibid)
{
	ET_START_ITERATION(_vaos, auto, i)
	{
		if ((i->vertexBuffer()->objectName() == vbid) && (i->indexBuffer()->objectName() == ibid))
			return i;
	}
	ET_END_ITERATION

	return VertexArrayObject();
}

