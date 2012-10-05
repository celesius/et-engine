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
		template <typename T>
		struct DefaultMaterialEntryBase
		{
		public:
			T value;
			size_t set;

		public:
			DefaultMaterialEntryBase() : value(0), set(0) { }

		protected:
			DefaultMaterialEntryBase(const T& v) : value(v), set(1) { }
		};

		template <typename T>
		struct DefaultMaterialEntry : public DefaultMaterialEntryBase<T>
		{
			DefaultMaterialEntry& operator = (T r) 
			{
				value = r;
				set = r != 0;
				return *this;
			}
		};

		template <> struct DefaultMaterialEntry<std::string> : public DefaultMaterialEntryBase<std::string>
		{
			DefaultMaterialEntry() : DefaultMaterialEntryBase(std::string()) { }

			DefaultMaterialEntry& operator = (const std::string& r)
			{
				value = r;
				set = (r.size() > 0) ? 1 : 0;
				return *this;
			}
		};

		template <> struct DefaultMaterialEntry<vec4> : public DefaultMaterialEntryBase<vec4>
		{
			DefaultMaterialEntry& operator = (const vec4& r)
			{
				value = r;
				set = (r.dotSelf() > 0.0f) ? 1 : 0;
				return *this;
			}
		};

		template <> struct DefaultMaterialEntry<Texture> : public DefaultMaterialEntryBase<Texture>
		{
			DefaultMaterialEntry& operator = (const Texture& r)
			{
				value = r;
				set = r.valid() ? 1 : 0;
				return *this;
			}
		};

		template <typename T>
		struct DefaultParameters
		{
			DefaultMaterialEntry<T> values[MaterialParameter_max];

			DefaultMaterialEntry<T>& operator[] (int i)
			{
				assert((i >= 0) && (i < MaterialParameter_max));
				return values[i];
			}

			const DefaultMaterialEntry<T>& operator[] (int i) const
			{
				assert((i >= 0) && (i < MaterialParameter_max));
				return values[i];
			}

			DefaultParameters& operator = (const DefaultParameters& r)
			{
				for (size_t i = 0; i < MaterialParameter_max; ++i)
					values[i] = r.values[i];
				return *this;
			}
		};

		typedef std::map<size_t, int> CustomIntParameters;
		typedef std::map<size_t, float> CustomFloatParameters;
		typedef std::map<size_t, vec4> CustomVectorParameters;
		typedef std::map<size_t, Texture> CustomTextureParameters;
		typedef std::map<size_t, std::string> CustomStringParameters;

		typedef DefaultParameters<int> DefaultIntParameters;
		typedef DefaultParameters<float> DefaultFloatParameters;
		typedef DefaultParameters<vec4> DefaultVectorParameters;
		typedef DefaultParameters<Texture> DefaultTextureParameters;
		typedef DefaultParameters<std::string> DefaultStringParameters;

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