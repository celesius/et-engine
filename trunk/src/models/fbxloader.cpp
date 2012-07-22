/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/platform/platform.h>

#if defined(ET_PLATFORM_WIN)

#include <iostream>
#include <assert.h>
#include <fbxsdk.h>

#include <et/app/application.h>
#include <et/rendering/rendercontext.h>
#include <et/apiobjects/vertexbuffer.h>
#include <et/vertexbuffer/IndexArray.h>
#include <et/primitives/primitives.h>
#include <et/resources/textureloader.h>
#include <et/scene3d/supportmesh.h>
#include <et/models/fbxloader.h>

#pragma comment(lib, "wininet.lib")

#if (ET_DEBUG)
	#pragma comment(lib, "fbxsdk-2013.2-mtd.lib")
#else
	#pragma comment(lib, "fbxsdk-2013.2-mt.lib")
#endif

using namespace FBXSDK_NAMESPACE;

const std::string s_supportMeshProperty = "support=true";
const std::string s_collisionMeshProperty = "collision=true";
const std::string s_lodMeshProperty = "lod=";

namespace et
{
	class FBXLoaderPrivate
	{
	private:
		RenderContext* _rc;
		TextureCache& _texCache;
		std::string _folder;

	public:
		FbxManager* manager;
		FbxImporter* importer;
		FbxScene* scene;

		s3d::Scene3dStorage::Pointer storage;
		
	public:
		FBXLoaderPrivate(RenderContext* rc, TextureCache& textureCache);
		~FBXLoaderPrivate();

		s3d::ElementContainer::Pointer parse();

		bool import(const std::string& filename);
		void loadTextures();
		void loadNode(FbxNode* node, s3d::Element::Pointer parent);
		void buildVertexBuffers(RenderContext* rc, s3d::Element::Pointer root);

		s3d::Mesh::Pointer loadMesh(FbxMesh* mesh, s3d::Element::Pointer parent, const MaterialList& materials, const StringList& params);
		Material loadMaterial(FbxSurfaceMaterial* material);

		void loadMaterialColorValue(Material& m, const std::string& propName, FbxSurfaceMaterial* fbxm, 
			const char* fbxprop, const char* fbxpropfactor);

		void loadMaterialValue(Material& m, const std::string& propName, FbxSurfaceMaterial* fbxm, 
			const char* fbxprop);

		void loadMaterialTextureValue(Material& m, const std::string& propName, FbxSurfaceMaterial* fbxm, 
			const char* fbxprop);

		StringList loadNodeProperties(FbxNode* node);
	};
}

using namespace et;

/* 
* Private implementation
*/ 

FBXLoaderPrivate::FBXLoaderPrivate(RenderContext* rc, TextureCache& textureCache) : manager(FbxManager::Create()), 
	_rc(rc), _texCache(textureCache)
{
	scene = FbxScene::Create(manager, 0);
}

FBXLoaderPrivate::~FBXLoaderPrivate()
{
	std::cout << "~FBXLoaderPrivate()" << std::endl;

	if (manager)
		manager->Destroy();
}

bool FBXLoaderPrivate::import(const std::string& filename)
{
	_folder = getFilePath(filename);

	int lFileMajor = 0;
	int lFileMinor = 0;
	int lFileRevision = 0;
	int lSDKMajor = 0;
	int lSDKMinor = 0;
	int lSDKRevision = 0;;

	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);
	importer = FbxImporter::Create(manager, 0);
	bool status = importer->Initialize(filename.c_str(), -1, manager->GetIOSettings());
	importer->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if (!status)
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n", importer->GetLastErrorString());

		if (importer->GetLastErrorID() == FbxIO::eFileVersionNotSupportedYet ||
			importer->GetLastErrorID() == FbxIO::eFileVersionNotSupportedAnymore)
		{
			printf("FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			printf("FBX version number for file %s is %d.%d.%d\n\n", filename.c_str(), lFileMajor, lFileMinor, lFileRevision);
		}
		importer->Destroy();
		return false;
	}

	status = importer->IsFBX();
	if (!status)
	{
		std::cout << "FBXLoader error: " << filename << " isn't FBX file" << std::endl;
		importer->Destroy();
		return false;
	}

	status = importer->Import(scene);
	if (!status)
	{
		std::cout << "FBXLoader error: unable to import scene from from" << filename << std::endl;
		importer->Destroy();
		return false;
	}

	importer->Destroy();
	return true;
}

s3d::ElementContainer::Pointer FBXLoaderPrivate::parse()
{
	s3d::ElementContainer::Pointer result(new s3d::ElementContainer("_fbx", 0));
	storage = s3d::Scene3dStorage::Pointer(new s3d::Scene3dStorage("_fbx_storage", result.ptr()));

	FbxAxisSystem targetAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);

	int upAxis = scene->GetGlobalSettings().GetOriginalUpAxis();
	if (upAxis != 1)
		targetAxisSystem.ConvertScene(scene);

	FbxSystemUnit sceneSystemUnit = scene->GetGlobalSettings().GetOriginalSystemUnit();
	if (sceneSystemUnit.GetScaleFactor() != 1.0f)
		FbxSystemUnit::cm.ConvertScene(scene);

	FbxNode* root = scene->GetRootNode(); 
	root->ResetPivotSetAndConvertAnimation();

	loadTextures();
	loadNode(root, result);
	buildVertexBuffers(_rc, result);

	return result;
}

void FBXLoaderPrivate::loadTextures()
{
	int textures = scene->GetTextureCount();
	for (int i = 0; i < textures; ++i)
	{
		FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(scene->GetTexture(i));
		if (fileTexture && !fileTexture->GetUserDataPtr())
		{
			std::string originalName = removeFileExt(fileTexture->GetRelativeFileName());
			std::string fileName = _folder + originalName;

			fileName = TextureLoader::resolveFileName(fileName, 0, true);
			if (!fileExists(fileName))
				fileName = TextureLoader::resolveFileName(_folder + getFileName(originalName), 0, true);

			Texture tex = _rc->textureFactory().loadTexture(fileName, _texCache);
			storage->addTexture(tex);
			fileTexture->SetUserDataPtr(tex.ptr());
		}
	}
}

void FBXLoaderPrivate::loadNode(FbxNode* node, s3d::Element::Pointer parent)
{
	MaterialList materials;
	StringList props = loadNodeProperties(node);

	const int lMaterialCount = node->GetMaterialCount();
	for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex)
	{
		FbxSurfaceMaterial* lMaterial = node->GetMaterial(lMaterialIndex);
		MaterialData* storedMaterial = static_cast<MaterialData*>(lMaterial->GetUserDataPtr());
		if (storedMaterial == 0)
		{
			Material m = loadMaterial(lMaterial);
			materials.push_back(m);
			storage->addMaterial(m);
			lMaterial->SetUserDataPtr(m.ptr());
		}
		else
		{
			materials.push_back(Material(storedMaterial));
		}
	}

	s3d::Element::Pointer createdElement;

	FbxNodeAttribute* lNodeAttribute = node->GetNodeAttribute();
	if (lNodeAttribute)
	{
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh* mesh = node->GetMesh();
			if (!mesh->IsTriangleMesh())
			{
				std::cout << "Triangulating " << node->GetName() << "...";
				FbxGeometryConverter lConverter(node->GetFbxManager());
				lConverter.TriangulateInPlace(node);
				mesh = node->GetMesh();
				std::cout << " done." << std::endl;
			}
		
			s3d::Mesh* storedElement = static_cast<s3d::Mesh*>(mesh->GetUserDataPtr());
			if (storedElement == 0)
			{
				createdElement = loadMesh(mesh, parent, materials, props);
				mesh->SetUserDataPtr(createdElement.ptr());
			}
			else
			{
				s3d::Mesh* instance = 0;

				if (storedElement->type() == s3d::ElementType_Mesh)
					 instance = storedElement->duplicate();
				else if (storedElement->type() == s3d::ElementType_SupportMesh)
					s3d::SupportMesh* instance = static_cast<s3d::SupportMesh*>(storedElement->duplicate());

				createdElement.reset(instance);
			}
		}
	}

	if (!createdElement.valid())
		createdElement = s3d::ElementContainer::Pointer(new s3d::ElementContainer(node->GetName(), parent.ptr()));

	const FbxMatrix& fbxTransform = node->EvaluateLocalTransform();
	mat4 transform;
	for (int v = 0; v < 4; ++v)
	{
		for (int u = 0; u < 4; ++u)
			transform[v][u] = static_cast<float>(fbxTransform.Get(v, u));
	}

	createdElement->setTransform(transform);
	createdElement->setName(node->GetName());
	for (StringList::const_iterator i = props.begin(), e = props.end(); i != e; ++i)
		createdElement->addPropertyString(*i);

	int lChildCount = node->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
		loadNode(node->GetChild(lChildIndex), createdElement);
}

void FBXLoaderPrivate::loadMaterialColorValue(Material& m, const std::string& propName, FbxSurfaceMaterial* fbxm, 
	const char* fbxprop, const char* fbxpropfactor)
{
	FbxProperty value = fbxm->FindProperty(fbxprop);
	FbxProperty factor = fbxm->FindProperty(fbxpropfactor);
	if (value.IsValid())
	{
		FbxDouble3 data = value.Get<FbxDouble3>();
		if (factor.IsValid())
		{
			double factorData = factor.Get<double>();
			data[0] *= factorData;
			data[1] *= factorData;
			data[2] *= factorData;
		}
		m->setVec4(propName,  vec4(static_cast<float>(data[0]),static_cast<float>(data[1]),static_cast<float>(data[2]), 1.0f));
	}
}

void FBXLoaderPrivate::loadMaterialTextureValue(Material& m, const std::string& propName, FbxSurfaceMaterial* fbxm, const char* fbxprop)
{
	FbxProperty value = fbxm->FindProperty(fbxprop);
	if (value.IsValid())
	{
		int lTextureCount = value.GetSrcObjectCount<FbxFileTexture>();
		if (lTextureCount)
		{
			FbxFileTexture* lTexture = value.GetSrcObject<FbxFileTexture>(0);
			if (lTexture && lTexture->GetUserDataPtr())
			{
				TextureData* ptr = reinterpret_cast<TextureData*>(lTexture->GetUserDataPtr());
				m->setTexture(propName, Texture(ptr));
			} 
		}
	}
}

void FBXLoaderPrivate::loadMaterialValue(Material& m, const std::string& propName, FbxSurfaceMaterial* fbxm, const char* fbxprop)
{
	const FbxProperty value = fbxm->FindProperty(fbxprop);
	if (value.IsValid())
		m->setFloat(propName, static_cast<float>(value.Get<double>()));
}

Material FBXLoaderPrivate::loadMaterial(FbxSurfaceMaterial* mat)
{
	Material m;

	loadMaterialValue(m, MaterialParameter_Roughness, mat, FbxSurfaceMaterial::sShininess);

	loadMaterialColorValue(m, MaterialParameter_DiffuseColor, mat, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
	loadMaterialColorValue(m, MaterialParameter_AmbientColor, mat, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
	loadMaterialColorValue(m, MaterialParameter_EmissiveColor, mat, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissive);
	loadMaterialColorValue(m, MaterialParameter_SpecularColor, mat, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);

	loadMaterialTextureValue(m, MaterialParameter_DiffuseMap, mat, FbxSurfaceMaterial::sDiffuse);
	loadMaterialTextureValue(m, MaterialParameter_AmbientMap, mat, FbxSurfaceMaterial::sAmbient);
	loadMaterialTextureValue(m, MaterialParameter_EmissiveMap, mat, FbxSurfaceMaterial::sEmissive);
	loadMaterialTextureValue(m, MaterialParameter_SpecularMap, mat, FbxSurfaceMaterial::sSpecular);
	loadMaterialTextureValue(m, MaterialParameter_NormalMap, mat, FbxSurfaceMaterial::sNormalMap);

	if (!m->hasTexture(MaterialParameter_NormalMap))
		loadMaterialTextureValue(m, MaterialParameter_NormalMap, mat, FbxSurfaceMaterial::sBump);

	return m;
}

inline bool isWhiteSpaceSymbol(char c)
	{ return isWhitespaceChar(c); }

s3d::Mesh::Pointer FBXLoaderPrivate::loadMesh(FbxMesh* mesh, s3d::Element::Pointer parent, 
	const MaterialList& materials, const StringList& params)
{
	s3d::Element::Pointer element;

	const char* meshName = mesh->GetName();
	if (strlen(meshName) == 0)
		meshName = mesh->GetNode()->GetName();

	size_t lodIndex = 0;
	bool support = false;
	for (StringList::const_iterator i = params.begin(), e = params.end(); i != e; ++i)
	{
		std::string p = *i;
		lowercase(p);
		p.erase(std::remove_if(p.begin(), p.end(), [](char c){ return isWhitespaceChar(c); } ), p.end());

		if ((p.find(s_collisionMeshProperty) == 0) || (p.find(s_supportMeshProperty) == 0))
		{
			support = true;
			break;
		}

		if (p.find_first_of(s_lodMeshProperty) == 0)
		{
			std::string l = trim(p.substr(s_lodMeshProperty.size()));
			lodIndex = strToInt(l);
		}
	}

	int lPolygonCount = mesh->GetPolygonCount();
	int lPolygonVertexCount = lPolygonCount * 3;

	bool isContainer = false;
	int numMaterials = 0;
	s3d::Element* realParent = parent.ptr();
	
	FbxGeometryElementMaterial* material = mesh->GetElementMaterial();
	FbxLayerElementArrayTemplate<int>* materialIndices = 0;
	if (material)
	{
		numMaterials = 1;
		FbxGeometryElement::EMappingMode mapping = material->GetMappingMode();
		assert((mapping == FbxGeometryElement::eAllSame) || (mapping == FbxGeometryElement::eByPolygon));

		isContainer = mapping == FbxGeometryElement::eByPolygon;
		if (isContainer)
		{
			materialIndices = &mesh->GetElementMaterial()->GetIndexArray();
			assert(materialIndices->GetCount() == lPolygonCount);
			for (int i = 0; i < materialIndices->GetCount(); ++i)
				numMaterials = etMax(numMaterials, materialIndices->GetAt(i) + 1);
		}
	}

	if (isContainer)
	{
		element = s3d::ElementContainer::Pointer(new s3d::ElementContainer(mesh->GetNode()->GetName(), parent.ptr()));
		realParent = element.ptr();
	}
	else
	{
		if (support)
			element = s3d::SupportMesh::Pointer(new s3d::SupportMesh(meshName, realParent));
		else
			element = s3d::Mesh::Pointer(new s3d::Mesh(meshName, realParent));
	}

	bool hasNormal = mesh->GetElementNormalCount() > 0;
	bool hasUV = mesh->GetElementUVCount() > 0;
	bool hasTangents = mesh->GetElementTangentCount() > 0;
	bool hasSmoothingGroups = mesh->GetElementSmoothingCount() > 0;

	const FbxVector4* lControlPoints = mesh->GetControlPoints();
	FbxGeometryElementTangent* tangents = hasTangents ? mesh->GetElementTangent() : 0;
	FbxGeometryElementSmoothing* smoothing = hasSmoothingGroups ? mesh->GetElementSmoothing() : 0;

	if (hasNormal)
		hasNormal = mesh->GetElementNormal()->GetMappingMode() != FbxGeometryElement::eNone;

	if (hasUV)
		hasUV = mesh->GetElementUV()->GetMappingMode() != FbxGeometryElement::eNone;

	if (hasTangents)
		hasTangents = tangents->GetMappingMode() == FbxGeometryElement::eByPolygonVertex;

	if (hasSmoothingGroups)
		hasSmoothingGroups = smoothing->GetMappingMode() == FbxGeometryElement::eByPolygon;

	VertexDeclaration decl(true, Usage_Position, Type_Vec3);

	if (hasNormal)
		decl.push_back(Usage_Normal, Type_Vec3);

	if (hasUV)
		decl.push_back(Usage_TexCoord0, Type_Vec2);

	if (hasTangents)
		decl.push_back(Usage_Tangent, Type_Vec3);

	VertexArrayRef va = storage->vertexArrayWithDeclarationForAppendingSize(decl, lPolygonVertexCount);
	int vbIndex = storage->indexOfVertexArray(va);
	size_t vertexBaseOffset = va->size();
	va->increase(lPolygonVertexCount);

	IndexArrayRef ib = storage->indexArray();
	ib->resizeToFit(ib->actualSize() + lPolygonCount * 3);

	RawDataAcessor<vec3> pos = va->chunk(Usage_Position).accessData<vec3>(vertexBaseOffset);
	RawDataAcessor<vec3> nrm = va->chunk(Usage_Normal).accessData<vec3>(vertexBaseOffset);
	RawDataAcessor<vec2> uv = va->chunk(Usage_TexCoord0).accessData<vec2>(vertexBaseOffset);
	RawDataAcessor<vec3> tang = va->chunk(Usage_Tangent).accessData<vec3>(vertexBaseOffset);
	RawDataAcessor<int> smooth = va->smoothing().accessData<int>(vertexBaseOffset);

	FbxStringList lUVNames;
	mesh->GetUVSetNames(lUVNames);
	std::string uvSetName = (hasUV && lUVNames.GetCount()) ? lUVNames[0].Buffer() : std::string();

	int lastMaterialIndex = -1;
	int vertexCount = 0;
	size_t indexOffset = vertexBaseOffset;

#define ET_FBX_LOADER_PUSH_VERTEX FbxVector4 v = lControlPoints[lControlPointIndex]; \
		pos[vertexCount] = vec3(static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2]));

#define ET_FBX_LOADER_PUSH_NORMAL if (hasNormal) { \
		FbxVector4 n; mesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, n); \
		nrm[vertexCount] = vec3(static_cast<float>(n[0]), static_cast<float>(n[1]), static_cast<float>(n[2])); }

#define ET_FBX_LOADER_PUSH_UV if (hasUV) { FbxVector2 t; \
		mesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, uvSetName.c_str(), t); \
		uv[vertexCount] = vec2(static_cast<float>(t[0]), static_cast<float>(t[1]));	}

#define ET_FBX_LOADER_PUSH_TANGENT if (hasTangents) { FbxVector4 t; \
		if (tangents->GetReferenceMode() == FbxGeometryElement::eDirect) \
			t = tangents->GetDirectArray().GetAt(vertexCount); \
	else if (tangents->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) \
			t = tangents->GetDirectArray().GetAt(tangents->GetIndexArray().GetAt(vertexCount)); \
		tang[vertexCount] = vec3(static_cast<float>(t[0]), static_cast<float>(t[1]), static_cast<float>(t[2])); }

	if (isContainer)
	{
		for (int m = 0; m < numMaterials; ++m)
		{
			s3d::Mesh::Pointer meshElement;

			if (support)
				meshElement = s3d::SupportMesh::Pointer(new s3d::SupportMesh(meshName, realParent));
			else
				meshElement = s3d::Mesh::Pointer(new s3d::Mesh(meshName, realParent));

			meshElement->tag = vbIndex;
			meshElement->setStartIndex(indexOffset);
			meshElement->setMaterial(materials.at(m));
			for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
			{
				if (materialIndices->GetAt(lPolygonIndex) == m)
				{
					for (int lVerticeIndex = 0; lVerticeIndex < 3; ++lVerticeIndex)
					{
						const int lControlPointIndex = mesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
						ib->setIndex(vertexBaseOffset + vertexCount, indexOffset);
						ET_FBX_LOADER_PUSH_VERTEX
						ET_FBX_LOADER_PUSH_NORMAL
						ET_FBX_LOADER_PUSH_UV
						ET_FBX_LOADER_PUSH_TANGENT
						++vertexCount;
						++indexOffset;
					}
				}
			}

			meshElement->setNumIndexes(indexOffset - meshElement->startIndex());
			if ((lodIndex > 0) && (parent->type() == s3d::ElementType_Mesh))
			{
				s3d::Mesh::Pointer p = parent;
				p->attachLod(lodIndex, meshElement);
			}
		}
	}
	else
	{
		s3d::Mesh* me = static_cast<s3d::Mesh*>(element.ptr());
		me->tag = vbIndex;
		me->setStartIndex(vertexBaseOffset);
		me->setNumIndexes(lPolygonVertexCount);
		if (materials.size())
			me->setMaterial(materials.front());
		
		for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
		{
			for (int lVerticeIndex = 0; lVerticeIndex < 3; ++lVerticeIndex)
			{
				const int lControlPointIndex = mesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
				ib->setIndex(vertexBaseOffset + vertexCount, indexOffset);
				ET_FBX_LOADER_PUSH_VERTEX
				ET_FBX_LOADER_PUSH_NORMAL
				ET_FBX_LOADER_PUSH_UV
				ET_FBX_LOADER_PUSH_TANGENT

				if (hasSmoothingGroups)
				{
					int sgIndex = 0;
					if (smoothing->GetReferenceMode() == FbxGeometryElement::eDirect)
						sgIndex = lPolygonIndex;
					else if (smoothing->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						sgIndex = smoothing->GetIndexArray().GetAt(vertexCount);
					sgIndex = smoothing->GetDirectArray().GetAt(sgIndex);
					smooth[vertexCount] = sgIndex;
				}

				++vertexCount;
				++indexOffset;
			}
		}

		if ((lodIndex > 0) && (parent->type() == s3d::ElementType_Mesh))
		{
			s3d::Mesh::Pointer p = parent;
			p->attachLod(lodIndex, element);
		}
	}

	return element;
}

void FBXLoaderPrivate::buildVertexBuffers(RenderContext* rc, s3d::Element::Pointer root)
{
	IndexBuffer primaryIndexBuffer = rc->vertexBufferFactory().createIndexBuffer("fbx-i", storage->indexArray(), BufferDrawType_Static);

	std::vector<VertexArrayObject> vertexArrayObjects;
	VertexArrayList& vertexArrays = storage->vertexArrays();
	for (VertexArrayList::const_iterator i = vertexArrays.begin(), e = vertexArrays.end(); i != e; ++i)
	{
		VertexArrayObject vao = rc->vertexBufferFactory().createVertexArrayObject("fbx-vao");
		VertexBuffer vb = rc->vertexBufferFactory().createVertexBuffer("fbx-v", *i, BufferDrawType_Static);
		vao->setBuffers(vb, primaryIndexBuffer);
		vertexArrayObjects.push_back(vao);
	}

	s3d::Element::List meshes = root->childrenOfType(s3d::ElementType_Mesh);
	for (s3d::Element::List::iterator i = meshes.begin(), e = meshes.end(); i != e; ++i)
	{
		s3d::Mesh* mesh = static_cast<s3d::Mesh*>(i->ptr());
		mesh->setVertexArrayObject(vertexArrayObjects[mesh->tag]);
	}

	for (s3d::Element::List::iterator i = meshes.begin(), e = meshes.end(); i != e; ++i)
	{
		s3d::Mesh* mesh = static_cast<s3d::Mesh*>(i->ptr());
		mesh->cleanupLodChildren();
	}

	meshes = root->childrenOfType(s3d::ElementType_SupportMesh);
	for (s3d::Element::List::iterator i = meshes.begin(), e = meshes.end(); i != e; ++i)
	{
		s3d::SupportMesh* mesh = static_cast<s3d::SupportMesh*>(i->ptr());
		mesh->setVertexArrayObject(vertexArrayObjects[mesh->tag]);
		mesh->fillCollisionData(vertexArrays[mesh->tag], storage->indexArray());
	}

	rc->renderState().bindVertexArray(0);
	rc->renderState().bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	rc->renderState().bindBuffer(GL_ARRAY_BUFFER, 0);
}

StringList FBXLoaderPrivate::loadNodeProperties(FbxNode* node)
{
	StringList result;
	FbxProperty prop = node->GetFirstProperty();
	while (prop.IsValid())
	{
		if ((true || prop.GetFlag(FbxPropertyAttr::eUser)) && (prop.GetPropertyDataType().GetType() == eFbxString))
		{
			FbxString str = prop.Get<FbxString>();
			size_t len = str.GetLen();

			BinaryDataStorage line(len+1, 0);
			for (size_t i = 0; i < len; ++i)
			{
				char c = str[i];
				if ((c == 0x0a) || (c == 0x0d))
				{
					if (line.currentIndex())
						result.push_back(line.binary());

					line.setOffset(0);
					line.fill(0);
				}
				else
				{
					line.push_back(c);
				}
			}

			if (line.currentIndex())
				result.push_back(line.binary());
		}

		prop = node->GetNextProperty(prop);
	};

	return result;
}

/*
* Base objects
*/

FBXLoader::FBXLoader(const std::string& filename) : _filename(filename)
{
}

FBXLoader::~FBXLoader()
{
}

s3d::ElementContainer::Pointer FBXLoader::load(RenderContext* rc, TextureCache& textureCache)
{
	s3d::ElementContainer::Pointer result;
	FBXLoaderPrivate* loader = new FBXLoaderPrivate(rc, textureCache);

	if (loader->import(_filename))
		result = loader->parse();

	mainRunLoop()->addTask(new DeletionTask<FBXLoaderPrivate>(loader));
	return result;
}

#endif