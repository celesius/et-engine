/*
* This file is part of `et engine`
* Copyright 2009-2012 by Sergey Reznik
* Please, do not modify contents without approval.
*
*/

#pragma once

#include <et/rendering/renderstate.h>
#include <et/scene3d/material.parameters.h>

namespace et
{
	class MaterialData : public APIObjectData
	{
	public:
		MaterialData();
		MaterialData(std::istream& stream, RenderContext* rc, TextureCache& cache, const std::string& texturesBasePath);

		const int getInt(size_t param) const;
		const float getFloat(size_t param) const;
		const vec4& getVector(size_t param) const;
		const std::string& getString(size_t param) const;
		const Texture& getTexture(size_t param) const;

		void setInt(size_t param, int value);
		void setFloat(size_t param, float value);
		void setVector(size_t param, const vec4& value);
		void setTexture(size_t param, const Texture& value);
		void setString(size_t param, const std::string& value);

		bool hasInt(size_t param) const;
		bool hasFloat(size_t param) const;
		bool hasVector(size_t param) const;
		bool hasTexture(size_t param) const;
		bool hasString(size_t param) const;

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
		void deserialize3(std::istream& stream, RenderContext* rc, TextureCache& cache, const std::string& texturesBasePath);

	private:
		DefaultIntParameters _defaultIntParameters;
		DefaultFloatParameters _defaultFloatParameters;
		DefaultVectorParameters _defaultVectorParameters;
		DefaultTextureParameters _defaultTextureParameters;
		DefaultStringParameters _defaultStringParameters;

		CustomIntParameters _customIntParameters;
		CustomFloatParameters _customFloatParameters;
		CustomVectorParameters _customVectorParameters;
		CustomTextureParameters _customTextureParameters;
		CustomStringParameters _customStringParameters;

		BlendState _blend;
		bool _depthMask;
	};

	class Material : public IntrusivePtr<MaterialData>
	{
	public:
		typedef std::vector<Material> List;

	public:
		Material() : IntrusivePtr<MaterialData>(new MaterialData()) 
			{ }

		explicit Material(MaterialData* data) : IntrusivePtr<MaterialData>(data) 
			{ }
	};

	inline bool operator < (const Material& m1, const Material& m2)
		{ return reinterpret_cast<size_t>(m1.ptr()) < reinterpret_cast<size_t>(m2.ptr()); }
}