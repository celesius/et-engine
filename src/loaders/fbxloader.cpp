#include <et/platform/platform.h>

#if !defined(ET_PLATFORM_WIN)
	#error FBXLoader requiers Windows environment
#endif

#include <iostream>
#include <assert.h>
#include <fbxsdk.h>

#include <et/app/application.h>
#include <et/device/rendercontext.h>
#include <et/apiobjects/vertexbuffer.h>
#include <et/vertexbuffer/IndexArray.h>
#include <et/primitives/primitives.h>
#include <et/resources/textureloader.h>
#include <et/scene3d/collisionmesh.h>
#include <et/loaders/fbxloader.h>

#pragma comment(lib, "wininet.lib")

#if (ET_DEBUG)
	#pragma comment(lib, "fbxsdk-2012.2-mtd.lib")
#else
	#pragma comment(lib, "fbxsdk-2012.2-mt.lib")
#endif

using namespace FBXSDK_NAMESPACE;

const std::string s_collisionMeshProperty = "collision = true";
const std::string s_lodMeshProperty = "lod = ";

namespace et
{
	class FBXLoaderPrivate
	{
	private:
		RenderContext* _rc;
		TextureCache& _texCache;
		std::string _folder;

	public:
		KFbxSdkManager* manager;
		KFbxImporter* importer;
		KFbxScene* scene;

		s3d::Scene3dStorage::Pointer storage;
		
	public:
		FBXLoaderPrivate(RenderContext* rc, TextureCache& textureCache);
		~FBXLoaderPrivate();

		s3d::ElementContainer::Pointer parse();

		bool import(const std::string& filename);
		void loadTextures();
		void loadNode(KFbxNode* node, s3d::Element::Pointer parent);
		void buildVertexBuffers(RenderContext* rc, s3d::Element::Pointer root);

		s3d::Mesh::Pointer loadMesh(KFbxMesh* mesh, s3d::Element::Pointer parent, const MaterialList& materials, const StringList& params);
		Material loadMaterial(KFbxSurfaceMaterial* material);

		void loadMaterialColorValue(Material& m, const std::string& propName, KFbxSurfaceMaterial* fbxm, 
			const char* fbxprop, const char* fbxpropfactor);

		void loadMaterialValue(Material& m, const std::string& propName, KFbxSurfaceMaterial* fbxm, 
			const char* fbxprop);

		void loadMaterialTextureValue(Material& m, const std::string& propName, KFbxSurfaceMaterial* fbxm, 
			const char* fbxprop);

		StringList loadNodeProperties(KFbxNode* node);
	};
}

using namespace et;

/* 
* Private implementation
*/ 

FBXLoaderPrivate::FBXLoaderPrivate(RenderContext* rc, TextureCache& textureCache) : manager(KFbxSdkManager::Create()), 
	_rc(rc), _texCache(textureCache)
{
	scene = KFbxScene::Create(manager, 0);
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

	KFbxSdkManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);
	importer = KFbxImporter::Create(manager, 0);
	bool status = importer->Initialize(filename.c_str(), -1, manager->GetIOSettings());
	importer->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if (!status)
	{
		printf("Call to KFbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n", importer->GetLastErrorString());

		if (importer->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_YET ||
			importer->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_ANYMORE)
		{
			printf("FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			printf("FBX version number for file %s is %d.%d.%d\n\n", filename, lFileMajor, lFileMinor, lFileRevision);
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

	KFbxAxisSystem targetAxisSystem(KFbxAxisSystem::YAxis, KFbxAxisSystem::ParityOdd, KFbxAxisSystem::RightHanded);

	int upAxis = scene->GetGlobalSettings().GetOriginalUpAxis();
	if (upAxis != 1)
		targetAxisSystem.ConvertScene(scene);

	KFbxSystemUnit sceneSystemUnit = scene->GetGlobalSettings().GetOriginalSystemUnit();
	if (sceneSystemUnit.GetScaleFactor() != 1.0f)
		KFbxSystemUnit::cm.ConvertScene(scene);

	KFbxNode* root = scene->GetRootNode(); 
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
		KFbxFileTexture* fileTexture = KFbxCast<KFbxFileTexture>(scene->GetTexture(i));
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

void FBXLoaderPrivate::loadNode(KFbxNode* node, s3d::Element::Pointer parent)
{
	MaterialList materials;
	StringList props = loadNodeProperties(node);

	const int lMaterialCount = node->GetMaterialCount();
	for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex)
	{
		KFbxSurfaceMaterial* lMaterial = node->GetMaterial(lMaterialIndex);
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

	KFbxNodeAttribute* lNodeAttribute = node->GetNodeAttribute();
	if (lNodeAttribute)
	{
		if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eMESH)
		{
			KFbxMesh* mesh = node->GetMesh();
			if (!mesh->IsTriangleMesh())
			{
				std::cout << "Triangulating " << node->GetName() << "...";
				KFbxGeometryConverter lConverter(node->GetFbxSdkManager());
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
				else if (storedElement->type() == s3d::ElementType_CollisionMesh)
					s3d::CollisionMesh* instance = static_cast<s3d::CollisionMesh*>(storedElement->duplicate());

				createdElement.reset(instance);
			}
		}
	}

	if (!createdElement.valid())
		createdElement = s3d::ElementContainer::Pointer(new s3d::ElementContainer(node->GetName(), parent.ptr()));

	const KFbxMatrix& fbxTransform = node->EvaluateLocalTransform();
	mat4 transform;
	for (int v = 0; v < 4; ++v)
		for (int u = 0; u < 4; ++u)
			transform[v][u] = static_cast<float>(fbxTransform.Get(v, u));

	createdElement->setTransform(transform);
	createdElement->setName(node->GetName());

	int lChildCount = node->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
		loadNode(node->GetChild(lChildIndex), createdElement);
}

void FBXLoaderPrivate::loadMaterialColorValue(Material& m, const std::string& propName, KFbxSurfaceMaterial* fbxm, 
	const char* fbxprop, const char* fbxpropfactor)
{
	KFbxProperty value = fbxm->FindProperty(fbxprop);
	KFbxProperty factor = fbxm->FindProperty(fbxpropfactor);
	if (value.IsValid())
	{
		fbxDouble3 data = KFbxGet<fbxDouble3>(value);
		if (factor.IsValid())
		{
			double factorData = KFbxGet<double>(factor);
			data[0] *= factorData;
			data[1] *= factorData;
			data[2] *= factorData;
		}
		m->setVec4(propName,  vec4(static_cast<float>(data[0]),static_cast<float>(data[1]),static_cast<float>(data[2]), 1.0f));
	}
}

void FBXLoaderPrivate::loadMaterialTextureValue(Material& m, const std::string& propName, KFbxSurfaceMaterial* fbxm, const char* fbxprop)
{
	KFbxProperty value = fbxm->FindProperty(fbxprop);
	if (value.IsValid())
	{
		int lTextureCount = value.GetSrcObjectCount(KFbxFileTexture::ClassId);
		if (lTextureCount)
		{
			KFbxFileTexture* lTexture = value.GetSrcObject(FBX_TYPE(KFbxFileTexture), 0);
			if (lTexture && lTexture->GetUserDataPtr())
			{
				TextureData* ptr = reinterpret_cast<TextureData*>(lTexture->GetUserDataPtr());
				m->setTexture(propName, Texture(ptr));
			} 
		}
	}
}

void FBXLoaderPrivate::loadMaterialValue(Material& m, const std::string& propName, KFbxSurfaceMaterial* fbxm, const char* fbxprop)
{
	const KFbxProperty value = fbxm->FindProperty(fbxprop);
	if (value.IsValid())
		m->setFloat(propName, static_cast<float>(KFbxGet<double>(value)));
}

Material FBXLoaderPrivate::loadMaterial(KFbxSurfaceMaterial* mat)
{
	Material m;

	loadMaterialValue(m, MaterialParameter_Roughness, mat, KFbxSurfaceMaterial::sShininess);

	loadMaterialColorValue(m, MaterialParameter_DiffuseColor, mat, KFbxSurfaceMaterial::sDiffuse, KFbxSurfaceMaterial::sDiffuseFactor);
	loadMaterialColorValue(m, MaterialParameter_AmbientColor, mat, KFbxSurfaceMaterial::sAmbient, KFbxSurfaceMaterial::sAmbientFactor);
	loadMaterialColorValue(m, MaterialParameter_EmissiveColor, mat, KFbxSurfaceMaterial::sEmissive, KFbxSurfaceMaterial::sEmissive);
	loadMaterialColorValue(m, MaterialParameter_SpecularColor, mat, KFbxSurfaceMaterial::sSpecular, KFbxSurfaceMaterial::sSpecularFactor);

	loadMaterialTextureValue(m, MaterialParameter_DiffuseMap, mat, KFbxSurfaceMaterial::sDiffuse);
	loadMaterialTextureValue(m, MaterialParameter_AmbientMap, mat, KFbxSurfaceMaterial::sAmbient);
	loadMaterialTextureValue(m, MaterialParameter_EmissiveMap, mat, KFbxSurfaceMaterial::sEmissive);
	loadMaterialTextureValue(m, MaterialParameter_SpecularMap, mat, KFbxSurfaceMaterial::sSpecular);
	loadMaterialTextureValue(m, MaterialParameter_NormalMap, mat, KFbxSurfaceMaterial::sNormalMap);

	if (!m->hasTexture(MaterialParameter_NormalMap))
		loadMaterialTextureValue(m, MaterialParameter_NormalMap, mat, KFbxSurfaceMaterial::sBump);

	return m;
}

s3d::Mesh::Pointer FBXLoaderPrivate::loadMesh(KFbxMesh* mesh, s3d::Element::Pointer parent, 
	const MaterialList& materials, const StringList& params)
{
	s3d::Element::Pointer element;

	size_t lodIndex = 0;
	bool collision = false;
	for (StringList::const_iterator i = params.begin(), e = params.end(); i != e; ++i)
	{
		const std::string& p = *i;
		if (p == s_collisionMeshProperty)
		{
			collision = true;
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
	
	KFbxGeometryElementMaterial* material = mesh->GetElementMaterial();
	KFbxLayerElementArrayTemplate<int>* materialIndices = 0;
	if (material)
	{
		numMaterials = 1;
		KFbxGeometryElement::EMappingMode mapping = material->GetMappingMode();
		assert((mapping == KFbxGeometryElement::eALL_SAME) || (mapping == KFbxGeometryElement::eBY_POLYGON));

		isContainer = mapping == KFbxGeometryElement::eBY_POLYGON;
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
		if (collision)
			element = s3d::CollisionMesh::Pointer(new s3d::CollisionMesh(mesh->GetName(), realParent));
		else
			element = s3d::Mesh::Pointer(new s3d::Mesh(mesh->GetName(), realParent));
	}

	bool hasNormal = mesh->GetElementNormalCount() > 0;
	bool hasUV = mesh->GetElementUVCount() > 0;
	bool hasTangents = mesh->GetElementTangentCount() > 0;
	bool hasSmoothingGroups = mesh->GetElementSmoothingCount() > 0;

	const KFbxVector4* lControlPoints = mesh->GetControlPoints();
	KFbxGeometryElementTangent* tangents = hasTangents ? mesh->GetElementTangent() : 0;
	KFbxGeometryElementSmoothing* smoothing = hasSmoothingGroups ? mesh->GetElementSmoothing() : 0;

	if (hasNormal)
		hasNormal = mesh->GetElementNormal()->GetMappingMode() != KFbxGeometryElement::eNONE;

	if (hasUV)
		hasUV = mesh->GetElementUV()->GetMappingMode() != KFbxGeometryElement::eNONE;

	if (hasTangents)
		hasTangents = tangents->GetMappingMode() == KFbxGeometryElement::eBY_POLYGON_VERTEX;

	if (hasSmoothingGroups)
		hasSmoothingGroups = smoothing->GetMappingMode() == KFbxGeometryElement::eBY_POLYGON;

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

	KStringList lUVNames;
	mesh->GetUVSetNames(lUVNames);
	std::string uvSetName = (hasUV && lUVNames.GetCount()) ? lUVNames[0].Buffer() : std::string();

	int lastMaterialIndex = -1;
	int vertexCount = 0;
	size_t indexOffset = vertexBaseOffset;

#define ET_FBX_LOADER_PUSH_VERTEX KFbxVector4 v = lControlPoints[lControlPointIndex]; \
		pos[vertexCount] = vec3(static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2]));

#define ET_FBX_LOADER_PUSH_NORMAL if (hasNormal) { \
		KFbxVector4 n; mesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, n); \
		nrm[vertexCount] = vec3(static_cast<float>(n[0]), static_cast<float>(n[1]), static_cast<float>(n[2])); }

#define ET_FBX_LOADER_PUSH_UV if (hasUV) { KFbxVector2 t; \
		mesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, uvSetName.c_str(), t); \
		uv[vertexCount] = vec2(static_cast<float>(t[0]), static_cast<float>(t[1]));	}

#define ET_FBX_LOADER_PUSH_TANGENT if (hasTangents) { KFbxVector4 t; \
		if (tangents->GetReferenceMode() == KFbxGeometryElement::eDIRECT) \
			t = tangents->GetDirectArray().GetAt(vertexCount); \
		else if (tangents->GetReferenceMode() == KFbxGeometryElement::eINDEX_TO_DIRECT) \
			t = tangents->GetDirectArray().GetAt(tangents->GetIndexArray().GetAt(vertexCount)); \
		tang[vertexCount] = vec3(static_cast<float>(t[0]), static_cast<float>(t[1]), static_cast<float>(t[2])); }

	if (isContainer)
	{
		for (int m = 0; m < numMaterials; ++m)
		{
			s3d::Mesh::Pointer meshElement;

			if (collision)
				meshElement = s3d::CollisionMesh::Pointer(new s3d::CollisionMesh(mesh->GetName(), realParent));
			else
				meshElement = s3d::Mesh::Pointer(new s3d::Mesh(mesh->GetName(), realParent));

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
					if (smoothing->GetReferenceMode() == KFbxGeometryElement::eDIRECT)
						sgIndex = lPolygonIndex;
					else if (smoothing->GetReferenceMode() == KFbxGeometryElement::eINDEX_TO_DIRECT)
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

	meshes = root->childrenOfType(s3d::ElementType_CollisionMesh);
	for (s3d::Element::List::iterator i = meshes.begin(), e = meshes.end(); i != e; ++i)
	{
		s3d::CollisionMesh* mesh = static_cast<s3d::CollisionMesh*>(i->ptr());
		mesh->setVertexArrayObject(vertexArrayObjects[mesh->tag]);
		mesh->fillCollisionData(vertexArrays[mesh->tag], storage->indexArray());
	}

	rc->renderState().bindVertexArray(0);
	rc->renderState().bindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	rc->renderState().bindBuffer(GL_ARRAY_BUFFER, 0);
}

StringList FBXLoaderPrivate::loadNodeProperties(KFbxNode* node)
{
	StringList result;
	KFbxProperty prop = node->GetFirstProperty();
	while (prop.IsValid())
	{
		if (prop.GetFlag(KFbxProperty::eUSER) && (prop.GetPropertyDataType().GetType() == eSTRING))
			result.push_back(KFbxGet<KString>(prop).Buffer());
		prop = node->GetNextProperty(prop);
	};

	for (StringList::iterator i = result.begin(), e = result.end(); i != e; ++i)
		std::cout << *i << std::endl;

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
