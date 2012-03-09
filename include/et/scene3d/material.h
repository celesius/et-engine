#pragma once

#include <list>
#include <map>
#include <et/rendering/renderstate.h>
#include <et/apiobjects/texture.h>
#include <et/device/rendercontext.h>

namespace et
{
	static const std::string MaterialParameter_AmbientColor = "ambient_color";
	static const std::string MaterialParameter_DiffuseColor = "diffuse_color";
	static const std::string MaterialParameter_SpecularColor = "specular_color";
	static const std::string MaterialParameter_EmissiveColor = "emissive_color";

	static const std::string MaterialParameter_AmbientMap = "ambient_map";
	static const std::string MaterialParameter_DiffuseMap = "diffuse_map";
	static const std::string MaterialParameter_SpecularMap = "specular_map";
	static const std::string MaterialParameter_EmissiveMap = "emissive_map";
	static const std::string MaterialParameter_NormalMap = "normalmap_map";
	static const std::string MaterialParameter_LightMap = "lightmap_map";

	static const std::string MaterialParameter_Roughness = "roughness";
	static const std::string MaterialParameter_Transparency = "transparency";
	static const std::string MaterialParameter_BumpFactor = "bump_factor";
	static const std::string MaterialParameter_IlluminationType = "illumination_type";

	typedef std::map<std::string, int> IntParameters;
	typedef std::map<std::string, float> FloatParameters;
	typedef std::map<std::string, vec4> VectorParameters;
	typedef std::map<std::string, Texture> TextureParameters;
	typedef std::map<std::string, std::string> StringParameters;

	class MaterialData : public APIObjectData
	{
	public:
		MaterialData();
		MaterialData(std::istream& stream, RenderContext* rc, TextureCache& cache);

		void setName(const std::string& name)
			{ APIObjectData::setName(name); }

		int& getInt(const std::string& param) 
			{ return _intParameters[param]; }

		float& getFloat(const std::string& param) 
			{ return _floatParameters[param]; }

		inline vec4& getVec4(const std::string& param) 
			{ return _vectorParameters[param]; }

		bool hasTexture(const std::string& param)
			{ return _textureParameters.find(param) != _textureParameters.end(); }

		Texture& getTexture(const std::string& param)
			{ return _textureParameters[param]; }

		std::string& getString(const std::string& param)
			{ return _stringParameters[param]; }

		const Texture& getTexture(const std::string& param) const 
			{ return _textureParameters.find(param)->second; }

		void setInt(const std::string& param, int value)
			{ _intParameters[param] = value; };

		void setFloat(const std::string& param, float value)
			{ _floatParameters[param] = value; };

		void setVec4(const std::string& param, const vec4& value)
			{ _vectorParameters[param] = value; };

		void setTexture(const std::string& param, const Texture& value)
			{ _textureParameters[param] = value; };

		void setString(const std::string& param, const std::string& value)
			{ _stringParameters[param] = value; };

		BlendState blendState() const
			{ return _blend; }

		void setBlendState(BlendState b) 
			{ _blend = b; }

		bool depthWriteEnabled() const
			{ return _depthMask; }

		void setDepthWriteEnabled(bool d) 
			{ _depthMask = d; }

		void serialize(std::ostream& stream) const;
		void deserialize(std::istream& stream, RenderContext* rc, TextureCache& cache);
		
		MaterialData* clone() const;

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