/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/rendering/renderstate.h>
#include <et/scene3d/serialization.h>
#include <et/scene3d/material.parameters.h>

namespace et
{
	namespace s3d
	{
		class MaterialData : public APIObject
		{
		public:
			MaterialData();
			
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

			void serialize(std::ostream& stream, StorageFormat format) const;

			void deserialize(std::istream& stream, RenderContext* rc, TextureCache& cache,
				const std::string& basePath, StorageFormat format);

			MaterialData* clone() const;

		public:
			ET_DECLARE_PROPERTY_GET_COPY_SET_COPY(BlendState, blendState, setBlendState)
			ET_DECLARE_PROPERTY_GET_COPY_SET_COPY(bool, depthWriteEnabled, setDepthWriteEnabled)

		private:
			void serializeBinary(std::ostream& stream) const;
			void serializeReadable(std::ostream& stream) const;

			void deserialize1(std::istream& stream, RenderContext* rc, TextureCache& cache,
				const std::string& texturesBasePath);

			void deserialize2(std::istream& stream, RenderContext* rc, TextureCache& cache,
				const std::string& texturesBasePath);

			void deserialize3(std::istream& stream, RenderContext* rc, TextureCache& cache,
				const std::string& texturesBasePath);

			/*
			 * Loading from XML
			 */
			void deserialize3FromXml(std::istream& stream, RenderContext* rc, TextureCache& cache,
				const std::string& texturesBasePath);

			void loadProperties(xmlNode*);

			void loadDefaultValues(xmlNode*, RenderContext* rc, TextureCache& cache,
				const std::string& basePath);
			
			void loadDefaultValue(xmlNode*, MaterialParameters, RenderContext* rc, TextureCache& cache,
				const std::string& basePath);

			Texture loadTexture(RenderContext* rc, const std::string& path,
				const std::string& basePath, TextureCache& cache);

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
		};

		class Material : public IntrusivePtr<MaterialData>
		{
		public:
			typedef std::vector<Material> List;

		public:
			Material() :
				IntrusivePtr<MaterialData>(new MaterialData()) { }

			explicit Material(MaterialData* data) :
				IntrusivePtr<MaterialData>(data) { }
		};

		typedef ObjectsCache MaterialCache;
	}
}