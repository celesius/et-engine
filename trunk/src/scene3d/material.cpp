/*
* This file is part of `et engine`
* Copyright 2009-2013 by Sergey Reznik
* Please, do not modify content without approval.
*
*/

#include <et/core/serialization.h>
#include <et/core/tools.h>
#include <et/rendering/rendercontext.h>
#include <et/scene3d/material.h>

using namespace et;

static const Texture _emptyTexture;
static const std::string _emptyString;
static const vec4 _emptyVector;

const std::string materialKeys[MaterialParameter_max] = 
{
	std::string(), 
	"ambient_color", "diffuse_color", "specular_color",
	"emissive_color", "ambient_map", "diffuse_map",
	"specular_map", "emissive_map", "normalmap_map",
	"lightmap_map", "roughness", "transparency",
	"bump_factor", "illumination_type",
};

const int MaterialVersion1_0_0 = 'MAT1';
const int MaterialVersion1_0_1 = 'MAT2';
const int MaterialVersion1_0_2 = 'MAT3';
const int MaterialCurrentVersion = MaterialVersion1_0_2;

inline size_t keyToMaterialParameter(const std::string& k)
{
	for (size_t i = 0; i < MaterialParameter_max; ++i)
	{
		if (k == materialKeys[i])
			return i;
	}

	return MaterialParameter_Undefined;
}

MaterialData::MaterialData() :
	APIObjectData("default"), _blend(Blend_Disabled), _depthMask(true)
{
	setVector(MaterialParameter_DiffuseColor, vec4(1.0f));
}

MaterialData::MaterialData(std::istream& stream, RenderContext* rc, TextureCache& cache,
	const std::string& texturesBasePath) : APIObjectData("default"), _blend(Blend_Disabled), _depthMask(true)
{
	deserialize(stream, rc, cache, texturesBasePath);
}

MaterialData* MaterialData::clone() const
{
	MaterialData* m = new MaterialData();
	m->tag = tag;

	m->_defaultIntParameters = _defaultIntParameters;
	m->_defaultFloatParameters = _defaultFloatParameters;
	m->_defaultVectorParameters = _defaultVectorParameters;
	m->_defaultTextureParameters = _defaultTextureParameters;
	m->_defaultStringParameters = _defaultStringParameters;

	m->_customIntParameters = _customIntParameters;
	m->_customFloatParameters = _customFloatParameters;
	m->_customVectorParameters = _customVectorParameters;
	m->_customTextureParameters = _customTextureParameters;
	m->_customStringParameters = _customStringParameters;

	m->_blend = _blend;
	m->_depthMask = _depthMask;
	return m;
}

void MaterialData::serialize(std::ostream& stream) const
{
	serializeInt(stream, MaterialCurrentVersion);
	serializeString(stream, name());
	serializeInt(stream, blendState());
	serializeInt(stream, depthWriteEnabled());

	serializeInt(stream, MaterialParameter_max);
	for (size_t i = 0; i < MaterialParameter_max; ++i)
	{
		const Texture& t = _defaultTextureParameters[i].value;

		serializeInt(stream, _defaultIntParameters[i].set);
		serializeInt(stream, _defaultIntParameters[i].value);

		serializeInt(stream, _defaultFloatParameters[i].set);
		serializeFloat(stream, _defaultFloatParameters[i].value);

		serializeInt(stream, _defaultVectorParameters[i].set);
		serializeVector(stream, _defaultVectorParameters[i].value);

		serializeInt(stream, _defaultTextureParameters[i].set);
		serializeString(stream, t.valid() ? t->name() : std::string());

		serializeInt(stream, _defaultStringParameters[i].set);
		serializeString(stream, _defaultStringParameters[i].value);
	}

	serializeInt(stream, static_cast<int>(_customIntParameters.size()));
	ET_ITERATE(_customIntParameters, auto&, i, serializeInt(stream, i.first);
		serializeInt(stream, i.second))

	serializeInt(stream, static_cast<int>(_customFloatParameters.size()));
	ET_ITERATE(_customFloatParameters, auto&, i, serializeInt(stream, i.first);
		serializeFloat(stream, i.second))

	serializeInt(stream, static_cast<int>(_customVectorParameters.size()));
	ET_ITERATE(_customVectorParameters, auto&, i, serializeInt(stream, i.first);
		serializeVector(stream, i.second))

	serializeInt(stream, static_cast<int>(_customTextureParameters.size()));
	ET_ITERATE(_customTextureParameters, auto&, i, serializeInt(stream, i.first);
		std::string path = i.second.valid() ? i.second->name() : std::string();
		serializeInt(stream, i.first);
		serializeString(stream, path));

	serializeInt(stream, static_cast<int>(_customStringParameters.size()));
	ET_ITERATE(_customStringParameters, auto&, i, serializeInt(stream, i.first);
		serializeString(stream, i.second))
}

void MaterialData::deserialize(std::istream& stream, RenderContext* rc, TextureCache& cache,
	const std::string& texturesBasePath)
{
	int version = deserializeInt(stream);

	setName(deserializeString(stream));
	_blend = static_cast<BlendState>(deserializeInt(stream));
	_depthMask = deserializeInt(stream) != 0;

	if (version == MaterialVersion1_0_0)
		deserialize1(stream, rc, cache, texturesBasePath);
	else if (version == MaterialVersion1_0_1)
		deserialize2(stream, rc, cache, texturesBasePath);
	else if (version == MaterialVersion1_0_2)
		deserialize3(stream, rc, cache, texturesBasePath);
}

void MaterialData::deserialize1(std::istream& stream, RenderContext* rc, TextureCache& cache,
	const std::string& texturesBasePath)
{
	size_t count = deserializeInt(stream);
	for (size_t i = 0; i < count; ++i)
	{
		std::string param = deserializeString(stream);
		int value = deserializeInt(stream);
		setInt(keyToMaterialParameter(param), value);
	}

	count = deserializeInt(stream);
	for (size_t i = 0; i < count; ++i)
	{
		std::string param = deserializeString(stream);
		float value = deserializeFloat(stream);
		setFloat(keyToMaterialParameter(param), value);
	}

	count = deserializeInt(stream);
	for (size_t i = 0; i < count; ++i)
	{
		std::string param = deserializeString(stream);
		vec4 value = deserializeVector<vec4>(stream);
		setVector(keyToMaterialParameter(param), value);
	}

	count = deserializeInt(stream);
	for (size_t i = 0; i < count; ++i)
	{
		std::string param = deserializeString(stream);
		std::string path = deserializeString(stream);
		if (path.length())
		{
			Texture t = rc->textureFactory().loadTexture(path, cache);
			if (t.invalid())
			{
				std::string relativePath = texturesBasePath + getFileName(path);
				t = rc->textureFactory().loadTexture(relativePath, cache);
			}

			if (t.valid())
				setTexture(keyToMaterialParameter(param), t);
			else
				std::cout << "Unable to load texture: " << path << std::endl;
		}
	}

	count = deserializeInt(stream);
	for (size_t i = 0; i < count; ++i)
	{
		std::string param = deserializeString(stream);
		std::string value = deserializeString(stream);
		setString(keyToMaterialParameter(param), value);
	}
}

void MaterialData::deserialize2(std::istream& stream, RenderContext* rc, TextureCache& cache,
	const std::string& texturesBasePath)
{
	size_t count = deserializeInt(stream);
	for (size_t i = 0; i < count; ++i)
	{
		int param = deserializeInt(stream);
		int value = deserializeInt(stream);
		setInt(param, value);
	}

	count = deserializeInt(stream);
	for (size_t i = 0; i < count; ++i)
	{
		int param = deserializeInt(stream);
		float value = deserializeFloat(stream);
		setFloat(param, value);
	}

	count = deserializeInt(stream);
	for (size_t i = 0; i < count; ++i)
	{
		int param = deserializeInt(stream);
		vec4 value = deserializeVector<vec4>(stream);
		setVector(param, value);
	}

	count = deserializeInt(stream);
	for (size_t i = 0; i < count; ++i)
	{
		int param = deserializeInt(stream);
		std::string path = deserializeString(stream);
		if (path.length())
		{
			Texture t = rc->textureFactory().loadTexture(path, cache);
			if (t.invalid())
			{
				std::string relativePath = texturesBasePath + getFileName(path);
				t = rc->textureFactory().loadTexture(relativePath, cache);
			}

			if (t.valid())
				setTexture(param, t);
			else
				std::cout << "Unable to load texture: " << path << std::endl;
		}
	}

	count = deserializeInt(stream);
	for (size_t i = 0; i < count; ++i)
	{
		int param = deserializeInt(stream);
		std::string value = deserializeString(stream);
		setString(param, value);
	}
}

void MaterialData::deserialize3(std::istream& stream, RenderContext* rc, TextureCache& cache,
	const std::string& texturesBasePath)
{
	int numParameters = deserializeInt(stream);
	for (int i = 0; i < numParameters; ++i)
	{
		int has = deserializeInt(stream);
		int ival = deserializeInt(stream);
		if (has)
		{
			setInt(i, ival);
		}

		has = deserializeInt(stream);
		float fval = deserializeFloat(stream);
		if (has)
		{
			setFloat(i, fval);
		}

		has = deserializeInt(stream);
		vec4 vval = deserializeVector<vec4>(stream);
		if (has)
		{
			setVector(i, vval);
		}

		has = deserializeInt(stream);
		std::string tval = deserializeString(stream);
		if (has && tval.size()) 
		{
			tval = normalizeFilePath(tval);
			Texture t = rc->textureFactory().loadTexture(tval, cache);
			if (t.invalid())
			{
				std::string relativePath = texturesBasePath + getFileName(tval);
				t = rc->textureFactory().loadTexture(relativePath, cache);
			}

			if (t.valid())
				setTexture(i, t);
			else
				std::cout << "Unable to load texture: " << tval << std::endl;
		}

		has = deserializeInt(stream);
		std::string sval = deserializeString(stream);
		if (has)
		{
			setString(i, sval);
		}
	}

	deserialize2(stream, rc, cache, texturesBasePath);
}

/*
 * Setters / getters
 */

const int MaterialData::getInt(size_t param) const
{
	if (param < MaterialParameter_max)
		return _defaultIntParameters[param].value;

	auto i = _customIntParameters.find(param);
	return i == _customIntParameters.end() ? 0 : i->second; 
}

const float MaterialData::getFloat(size_t param) const
{ 
	if (param < MaterialParameter_max)
		return _defaultFloatParameters[param].value;

	auto i = _customFloatParameters.find(param);
	return i == _customFloatParameters.end() ? 0 : i->second; 
}

const vec4& MaterialData::getVector(size_t param) const
{ 
	if (param < MaterialParameter_max)
		return _defaultVectorParameters[param].value;

	auto i = _customVectorParameters.find(param);
	return i == _customVectorParameters.end() ? _emptyVector : i->second; 
}

const std::string& MaterialData::getString(size_t param) const
{ 
	if (param < MaterialParameter_max)
		return _defaultStringParameters[param].value;

	auto i = _customStringParameters.find(param);
	return i == _customStringParameters.end() ? _emptyString : i->second; 
}

const Texture& MaterialData::getTexture(size_t param) const 
{ 
	if (param < MaterialParameter_max)
		return _defaultTextureParameters[param].value;

	auto i = _customTextureParameters.find(param);
	return i == _customTextureParameters.end() ? _emptyTexture : i->second; 
}

void MaterialData::setInt(size_t param, int value)
{
	if (param < MaterialParameter_max)
		_defaultIntParameters[param] = value;
	else
		_customIntParameters[param] = value; 
}

void MaterialData::setFloat(size_t param, float value)
{
	if (param < MaterialParameter_max)
		_defaultFloatParameters[param] = value;
	else
		_customFloatParameters[param] = value; 
}

void MaterialData::setVector(size_t param, const vec4& value)
{
	if (param < MaterialParameter_max)
		_defaultVectorParameters[param] = value;
	else
		_customVectorParameters[param] = value; 
}

void MaterialData::setTexture(size_t param, const Texture& value)
{
	if (param < MaterialParameter_max)
		_defaultTextureParameters[param] = value;
	else
		_customTextureParameters[param] = value; 
}

void MaterialData::setString(size_t param, const std::string& value)
{
	if (param < MaterialParameter_max)
		_defaultStringParameters[param] = value;
	else
		_customStringParameters[param] = value; 
}

bool MaterialData::hasVector(size_t param) const
{ 
	return (param < MaterialParameter_max) ? (_defaultVectorParameters[param].set > 0) : 
		(_customVectorParameters.find(param) != _customVectorParameters.end());
}

bool MaterialData::hasFloat(size_t param) const
{
	return (param < MaterialParameter_max) ? (_defaultFloatParameters[param].set > 0) : 
		(_customFloatParameters.find(param) != _customFloatParameters.end());
}

bool MaterialData::hasTexture(size_t param) const
{ 
	return (param < MaterialParameter_max) ? (_defaultTextureParameters[param].set > 0) : 
		(_customTextureParameters.find(param) != _customTextureParameters.end()); 
}

bool MaterialData::hasInt(size_t param) const
{ 
	return (param < MaterialParameter_max) ? (_defaultIntParameters[param].set > 0) : 
		(_customIntParameters.find(param) != _customIntParameters.end()); 
}

bool MaterialData::hasString(size_t param) const
{ 
	return (param < MaterialParameter_max) ? (_defaultStringParameters[param].set > 0) : 
		(_customStringParameters.find(param) != _customStringParameters.end()); 
}
