/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/rendering/rendercontext.h>
#include <et/scene3d/material.h>
#include <et/core/serialization.h>

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
const int MaterialCurrentVersion = MaterialVersion1_0_1;

inline size_t keyToMaterialParameter(const std::string& k)
{
	for (size_t i = 0; i < MaterialParameter_max; ++i)
	{
		if (k == materialKeys[i])
			return i;
	}
	return MaterialParameter_Undefined;
}

MaterialData::MaterialData() : APIObjectData("default"), _blend(Blend_Disabled), _depthMask(true)
{
	setVec4(MaterialParameter_DiffuseColor, vec4(1.0f));
}

MaterialData::MaterialData(std::istream& stream, RenderContext* rc, TextureCache& cache, const std::string& texturesBasePath) :
	APIObjectData("default"), _blend(Blend_Disabled), _depthMask(true)
{
	deserialize(stream, rc, cache, texturesBasePath);
}

MaterialData* MaterialData::clone() const
{
	MaterialData* m = new MaterialData();
	m->_intParameters = _intParameters;
	m->_floatParameters = _floatParameters;
	m->_vectorParameters = _vectorParameters;
	m->_textureParameters = _textureParameters;
	m->_stringParameters = _stringParameters;
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

	serializeInt(stream, static_cast<int>(_intParameters.size()));
	for (IntParameters::const_iterator i = _intParameters.begin(), e = _intParameters.end(); i != e; ++i)
	{
		serializeInt(stream, i->first);
		serializeInt(stream, i->second);
	}

	serializeInt(stream, static_cast<int>(_floatParameters.size()));
	for (FloatParameters::const_iterator i = _floatParameters.begin(), e = _floatParameters.end(); i != e; ++i)
	{
		serializeInt(stream, i->first);
		serializeFloat(stream, i->second);
	}

	serializeInt(stream, static_cast<int>(_vectorParameters.size()));
	for (VectorParameters::const_iterator i = _vectorParameters.begin(), e = _vectorParameters.end(); i != e; ++i)
	{
		serializeInt(stream, i->first);
		serializeVector(stream, i->second);
	}

	serializeInt(stream, static_cast<int>(_textureParameters.size()));
	for (TextureParameters::const_iterator i = _textureParameters.begin(), e = _textureParameters.end(); i != e; ++i)
	{
		std::string path = i->second.valid() ? i->second->name() : std::string();
		serializeInt(stream, i->first);
		serializeString(stream, path);
	}

	serializeInt(stream, static_cast<int>(_stringParameters.size()));
	for (StringParameters::const_iterator i = _stringParameters.begin(), e = _stringParameters.end(); i != e; ++i)
	{
		serializeInt(stream, i->first);
		serializeString(stream, i->second);
	}

}

void MaterialData::deserialize(std::istream& stream, RenderContext* rc, TextureCache& cache, const std::string& texturesBasePath)
{
	int version = deserializeInt(stream);

	setName(deserializeString(stream));
	_blend = static_cast<BlendState>(deserializeInt(stream));
	_depthMask = deserializeInt(stream) != 0;

	if (version == MaterialVersion1_0_0)
		deserialize1(stream, rc, cache, texturesBasePath);
	else if (version == MaterialVersion1_0_1)
		deserialize2(stream, rc, cache, texturesBasePath);
}

void MaterialData::deserialize1(std::istream& stream, RenderContext* rc, TextureCache& cache, const std::string& texturesBasePath)
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
		setVec4(keyToMaterialParameter(param), value);
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
				path = texturesBasePath + getFileName(path);
				t = rc->textureFactory().loadTexture(path, cache, true);
			}
			setTexture(keyToMaterialParameter(param), t);
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

void MaterialData::deserialize2(std::istream& stream, RenderContext* rc, TextureCache& cache, const std::string& texturesBasePath)
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
		setVec4(param, value);
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
				path = texturesBasePath + getFileName(path);
				t = rc->textureFactory().loadTexture(path, cache, true);
			}
			setTexture(param, t);
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

const int MaterialData::getInt(size_t param) const
{
	IntParameters::const_iterator i = _intParameters.find(param);
	return i == _intParameters.end() ? 0 : i->second; 
}

const float MaterialData::getFloat(size_t param) const
{ 
	FloatParameters::const_iterator i = _floatParameters.find(param);
	return i == _floatParameters.end() ? 0 : i->second; 
}

const vec4& MaterialData::getVec4(size_t param) const
{ 
	VectorParameters::const_iterator i = _vectorParameters.find(param);
	return i == _vectorParameters.end() ? _emptyVector : i->second; 
}

const std::string& MaterialData::getString(size_t param) const
{ 
	StringParameters::const_iterator i = _stringParameters.find(param);
	return i == _stringParameters.end() ? _emptyString : i->second; 
}

const Texture& MaterialData::getTexture(size_t param) const 
{ 
	TextureParameters::const_iterator i = _textureParameters.find(param);
	return i == _textureParameters.end() ? _emptyTexture : i->second; 
}
