#include <et/app/application.h>
#include <et/device/rendercontext.h>
#include <et/threading/threading.h>
#include <et/apiobjects/texturefactory.h>
#include <et/resources/textureloader.h>
#include <et/apiobjects/texturedata.h>

using namespace et;

TextureFactory::TextureFactory(RenderContext* rc) : _rc(rc)
{
	_loadingThread = new TextureLoadingThread(this);
}

TextureFactory::~TextureFactory()
{
	_loadingThread->terminate();
}

Texture TextureFactory::loadTexture(const std::string& file, TextureCache& cache, bool async, TextureLoaderDelegate* delegate)
{
	CriticalSectionScope lock(_csTextureLoading);

	Texture texture = cache.findTexture(file);
	if (texture.isNull())
	{
		bool calledFromAnotherThread = Threading::currentThread() != threading().mainThread();

		size_t screenScale = _rc->screenScaleFactor();
		TextureDescription desc = async ? TextureLoader::loadDescription(file, screenScale, !calledFromAnotherThread) : 
			TextureLoader::load(file, screenScale);

		texture = Texture(new TextureData(_rc, desc, file, calledFromAnotherThread));
		cache.manageTexture(texture);

		if (async)
			_loadingThread->addRequest(file, _rc->screenScaleFactor(), texture, delegate);
		else if (calledFromAnotherThread)
			std::cout << "ERROR: Unable to load texture synchronously from secondary thread." << std::endl;
	}

	return texture;
}

Texture TextureFactory::genTexture(GLenum target, GLint internalformat, const vec2i& size, GLenum format, 
	GLenum type, const BinaryDataStorage& data, const std::string& id)
{
	TextureDescription desc;
	desc.target = target;
	desc.data = data;
	desc.format = format;
	desc.internalformat = internalformat;
	desc.type = type;
	desc.size = size;
	desc.mipMapCount = 1;
	desc.layersCount = 1;
	return Texture(new TextureData(_rc, desc, id, false));
}

Texture TextureFactory::genCubeTexture(GLint internalformat, GLsizei size, GLenum format, GLenum type, const std::string& id)
{
	TextureDescription desc;
	desc.target = GL_TEXTURE_CUBE_MAP;
	desc.format = format;
	desc.internalformat = internalformat;
	desc.type = type;
	desc.size = vec2i(size);
	desc.layersCount = 6;

	return Texture(new TextureData(_rc, desc, id, false));
}

Texture TextureFactory::genTexture(const TextureDescription& desc)
{
	return Texture(new TextureData(_rc, desc, desc.source, false));
}

Texture TextureFactory::genNoiseTexture(const vec2i& size, bool norm, const std::string& id)
{
	const float RAND_MAXF = static_cast<float>(RAND_MAX);

	DataStorage<vec4ub> randata(size.square());
	for (size_t i = 0; i < randata.size(); ++i)
	{ 
		vec4 rand_f = 2.0f * vec4(static_cast<float>(rand()) / RAND_MAXF, static_cast<float>(rand()) / RAND_MAXF, 
			static_cast<float>(rand()) / RAND_MAXF, static_cast<float>(rand()) / RAND_MAXF ) - vec4(1.0);

		if (norm)
			rand_f.xyz() = normalize( rand_f.xyz() );

		randata.push_back(vec4f_to_4ub(rand_f));
	}

	TextureDescription desc;
	desc.data = BinaryDataStorage(size.square() * 4);
	desc.target = GL_TEXTURE_2D;
	desc.format = GL_RGBA;
	desc.internalformat = GL_RGBA;
	desc.type = GL_UNSIGNED_BYTE;
	desc.size = size;
	desc.mipMapCount = 1;
	desc.layersCount = 1;
	memcpy(desc.data.raw(), randata.raw(), randata.dataSize());

	return Texture(new TextureData(_rc, desc, id, false));
}

void TextureFactory::textureLoadingThreadDidLoadTextureData(TextureLoadingRequest* request)
{
	CriticalSectionScope lock(_csTextureLoading);

	request->texture->updateData(_rc, *request->textureDescription);
	textureDidLoad.invoke(request->texture);

	if (request->delegate)
		request->delegate->textureDidLoad(request->texture);

	delete request;
}

Texture TextureFactory::loadTexturesToCubemap(const std::string& posx, const std::string& negx, const std::string& posy,
	const std::string& negy, const std::string& posz, const std::string& negz, TextureCache& cache)
{
	size_t screenScale = _rc->screenScaleFactor();
	TextureDescription layers[6] = 
	{
		TextureLoader::load(posx, screenScale), 
		TextureLoader::load(negx, screenScale), 
		TextureLoader::load(negy, screenScale), 
		TextureLoader::load(posy, screenScale), 
		TextureLoader::load(posz, screenScale), 
		TextureLoader::load(negz, screenScale) 
	};

	std::string texId = layers[0].source + ";";
	for (size_t l = 1; l < 6; ++l)
	{
		texId += (l < 5) ? layers[l].source + ";" : layers[l].source;
		if ((layers[l-1].size != layers[l].size) || 
			(layers[l-1].format != layers[l].format) ||
			(layers[l-1].internalformat != layers[l].internalformat) || 
			(layers[l-1].type != layers[l].type) || 
			(layers[l-1].mipMapCount != layers[l].mipMapCount) || 
			(layers[l-1].compressed != layers[l].compressed) ||
			(layers[l-1].data.size() != layers[l].data.size()))
		{
			std::cout << "Failed to load cubemap textures. Textures aren't identical:" << std::endl <<
				"> " << layers[l-1].source << std::endl <<
				"> " << layers[l].source << std::endl;
			return Texture();
		}
	}

	size_t layerSize = layers[0].dataSizeForAllMipLevels();
	TextureDescription desc;
	desc.target = GL_TEXTURE_CUBE_MAP;
	desc.layersCount = 6;
	desc.data.resize(6 * layerSize);

	for (size_t l = 0; l < 6; ++l)
		memcpy(desc.data.element_ptr(l * layerSize), layers[l].data.element_ptr(0), layerSize);

	desc.bitsPerPixel = layers[0].bitsPerPixel;
	desc.channels = layers[0].channels;
	desc.compressed = layers[0].compressed;
	desc.format = layers[0].format;
	desc.internalformat = layers[0].internalformat;
	desc.mipMapCount= layers[0].mipMapCount;
	desc.size = layers[0].size;
	desc.type = layers[0].type;

	Texture t(new TextureData(_rc, desc, texId, false));
	cache.manageTexture(t);
	return t;
}
