/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <list>
#include <map>

#include <et/apiobjects/texture.h>
#include <et/rendering/renderstate.h>

namespace et
{
	enum  MaterialParameters
	{
		MaterialParameter_Undefined,

		MaterialParameter_AmbientColor,
		MaterialParameter_DiffuseColor,
		MaterialParameter_SpecularColor,
		MaterialParameter_EmissiveColor,

		MaterialParameter_AmbientMap,	
		MaterialParameter_DiffuseMap,
		MaterialParameter_SpecularMap,
		MaterialParameter_EmissiveMap,
		MaterialParameter_NormalMap,	
		MaterialParameter_BumpMap,	
		MaterialParameter_ReflectionMap,

		MaterialParameter_AmbientFactor,
		MaterialParameter_DiffuseFactor,
		MaterialParameter_SpecularFactor,
		MaterialParameter_BumpFactor,	
		MaterialParameter_ReflectionFactor,	

		MaterialParameter_Roughness,
		MaterialParameter_Transparency,
		MaterialParameter_ShadingModel,

		MaterialParameter_max,
		MaterialParameter_User = 0xffff
	};

	typedef std::map<size_t, int> IntParameters;
	typedef std::map<size_t, float> FloatParameters;
	typedef std::map<size_t, vec4> VectorParameters;
	typedef std::map<size_t, Texture> TextureParameters;
	typedef std::map<size_t, std::string> StringParameters;

	class MaterialData : public APIObjectData
	{
	public:
		MaterialData();
		MaterialData(std::istream& stream, RenderContext* rc, TextureCache& cache, const std::string& texturesBasePath);

		const int getInt(size_t param) const;
		const float getFloat(size_t param) const;
		const vec4& getVec4(size_t param) const;
		const std::string& getString(size_t param) const;
		const Texture& getTexture(size_t param) const;

		void setName(const std::string& name)
			{ APIObjectData::setName(name); }

		void setInt(size_t param, int value)
			{ _intParameters[param] = value; };

		void setFloat(size_t param, float value)
			{ _floatParameters[param] = value; };

		void setVec4(size_t param, const vec4& value)
			{ _vectorParameters[param] = value; };

		void setTexture(size_t param, const Texture& value)
			{ _textureParameters[param] = value; };

		void setString(size_t param, const std::string& value)
			{ _stringParameters[param] = value; };

		bool hasTexture(size_t param)
			{ return _textureParameters.find(param) != _textureParameters.end(); }

		BlendState blendState() const
			{ return _blend; }

		void setBlendState(BlendState b) 
			{ _blend = b; }

		bool depthWriteEnabled() const
			{ return _depthMask; }

		void setDepthWriteEnabled(bool d) 
			{ _depthMask = d; }

		void serialize(std::ostream& stream) const;
		void deserialize(std::istream& stream, RenderContext* rc, TextureCache& cache, const std::string& texturesBasePath);
		
		MaterialData* clone() const;

	private:
		void deserialize1(std::istream& stream, RenderContext* rc, TextureCache& cache, const std::string& texturesBasePath);
		void deserialize2(std::istream& stream, RenderContext* rc, TextureCache& cache, const std::string& texturesBasePath);

	private:
		IntParameters _intParameters;
		FloatParameters _floatParameters;
		VectorParameters _vectorParameters;
		TextureParameters _textureParameters;
		StringParameters _stringParameters;
		BlendState _blend;
		bool _depthMask;
	};

	class Material : public IntrusivePtr<MaterialData>
	{
	public:
		Material() : IntrusivePtr<MaterialData>(new MaterialData()) 
			{ }

		explicit Material(MaterialData* data) : IntrusivePtr<MaterialData>(data) 
			{ }

	};

	inline bool operator < (const Material& m1, const Material& m2)
		{ return reinterpret_cast<size_t>(m1.ptr()) < reinterpret_cast<size_t>(m2.ptr()); }

	typedef std::vector<Material> MaterialList;
	typedef MaterialList::iterator MaterialIterator; 
}