#include <et/scene3d/material.h>
#include <et/core/serialization.h>

using namespace et;

const int MaterialVersion1_0_0 = 'MAT1';
const int MaterialCurrentVersion = MaterialVersion1_0_0;

void MaterialData::serialize(std::ostream& stream) const
{
	serializeInt(stream, MaterialCurrentVersion);
	serializeString(stream, name());

	serializeInt(stream, blendState());
	serializeInt(stream, depthWriteEnabled());

	serializeInt(stream, static_cast<int>(_intParameters.size()));
	for (IntParameters::const_iterator i = _intParameters.begin(), e = _intParameters.end(); i != e; ++i)
	{
		serializeString(stream, i->first);
		serializeInt(stream, i->second);
	}

	serializeInt(stream, static_cast<int>(_floatParameters.size()));
	for (FloatParameters::const_iterator i = _floatParameters.begin(), e = _floatParameters.end(); i != e; ++i)
	{
		serializeString(stream, i->first);
		serializeFloat(stream, i->second);
	}

	serializeInt(stream, static_cast<int>(_vectorParameters.size()));
	for (VectorParameters::const_iterator i = _vectorParameters.begin(), e = _vectorParameters.end(); i != e; ++i)
	{
		serializeString(stream, i->first);
		serializeVector(stream, i->second);
	}

	serializeInt(stream, static_cast<int>(_textureParameters.size()));
	for (TextureParameters::const_iterator i = _textureParameters.begin(), e = _textureParameters.end(); i != e; ++i)
	{
		std::string path = i->second.valid() ? i->second->name() : std::string();
		serializeString(stream, i->first);
		serializeString(stream, path);
	}

	serializeInt(stream, static_cast<int>(_stringParameters.size()));
	for (StringParameters::const_iterator i = _stringParameters.begin(), e = _stringParameters.end(); i != e; ++i)
	{
		serializeString(stream, i->first);
		serializeString(stream, i->second);
	}

}

void MaterialData::deserialize(std::istream& stream, RenderContext* rc, TextureCache& cache)
{
	int version = deserializeInt(stream);
	if (version != MaterialVersion1_0_0) return;

	setName(deserializeString(stream));
	_blend = static_cast<BlendState>(deserializeInt(stream));
	_depthMask = deserializeInt(stream) != 0;

	size_t count = deserializeInt(stream);
	for (size_t i = 0; i < count; ++i)
	{
		std::string param = deserializeString(stream);
		int value = deserializeInt(stream);
		setInt(param, value);
	}

	count = deserializeInt(stream);
	for (size_t i = 0; i < count; ++i)
	{
		std::string param = deserializeString(stream);
		float value = deserializeFloat(stream);
		setFloat(param, value);
	}

	count = deserializeInt(stream);
	for (size_t i = 0; i < count; ++i)
	{
		std::string param = deserializeString(stream);
		vec4 value = deserializeVector<vec4>(stream);
		setVec4(param, value);
	}

	count = deserializeInt(stream);
	for (size_t i = 0; i < count; ++i)
	{
		std::string param = deserializeString(stream);
		std::string path = deserializeString(stream);
		setTexture(param, rc->textureFactory().loadTexture(path, cache));
	}

	count = deserializeInt(stream);
	for (size_t i = 0; i < count; ++i)
	{
		std::string param = deserializeString(stream);
		std::string value = deserializeString(stream);
		setString(param, value);
	}
}

MaterialData::MaterialData() : APIObjectData("default"), _blend(Blend_Disabled), _depthMask(true)
{
	setVec4(MaterialParameter_DiffuseColor, vec4(1.0f));
}

MaterialData::MaterialData(std::istream& stream, RenderContext* rc, TextureCache& cache) : APIObjectData("default"),
	_blend(Blend_Disabled), _depthMask(true)
{
	deserialize(stream, rc, cache);
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