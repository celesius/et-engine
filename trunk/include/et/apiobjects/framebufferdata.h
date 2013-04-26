/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/apiobjects/apiobject.h>
#include <et/apiobjects/texture.h>
#include <et/apiobjects/texturefactory.h>

namespace et
{
	class RenderContext;
	struct FramebufferDescription
	{
		vec2i size;

		int numColorRenderTargets;
		int includeDepthRenderTarget;

		int colorInternalformat;
		uint32_t colorFormat;
		uint32_t colorType;

		int depthInternalformat;
		uint32_t depthFormat;
		uint32_t depthType;
		
		bool colorIsRenderbuffer;
		bool depthIsRenderbuffer;
		bool isCubemap;

		FramebufferDescription() : numColorRenderTargets(0), includeDepthRenderTarget(0), colorInternalformat(0), 
			colorFormat(0), colorType(0), depthInternalformat(0), depthFormat(0), depthType(0),
			colorIsRenderbuffer(false), depthIsRenderbuffer(false), isCubemap(false) { }
	};

	class FramebufferData : public APIObjectData
	{
	public:
		enum
		{
			MaxRenderTargets = 8
		};
		
	public:
		~FramebufferData();

		bool addRenderTarget(const Texture& texture);
		void addSameRendertarget();

		bool setDepthTarget(const Texture& texture);
		bool setDepthTarget(const Texture& texture, uint32_t target);

		bool setCurrentRenderTarget(const Texture& texture);
		bool setCurrentRenderTarget(const Texture& texture, uint32_t target);
		bool setCurrentRenderTarget(size_t index);
		
		bool setCurrentCubemapFace(size_t faceIndex);

		bool checkStatus();
		
		void setDrawBuffersCount(int c);
		
		size_t numRendertargets() const
			{ return _numTargets; }

		uint32_t glID() const
			{ return _id; }

		vec2i size() const
			{ return _size; }
	
		uint32_t colorRenderbuffer() const
			{ return _colorRenderbuffer; }
		
		uint32_t depthRenderbuffer() const
			{ return _depthRenderbuffer; }

		Texture renderTarget(size_t index = 0) const
			{ assert(index < MaxRenderTargets); return _renderTargets[index]; }
		
		Texture depthBuffer() const
			{ return _depthBuffer; }
		
		void setColorRenderbuffer(uint32_t r)
			{ _colorRenderbuffer = r; }
		
		void setDepthRenderbuffer(uint32_t r) 
			{ _depthRenderbuffer = r; }
		
		void forceSize(const vec2i&);

	private:
		friend class FramebufferFactory;
		FramebufferData(RenderContext* rc, TextureFactory* tf, const FramebufferDescription& desc, const std::string& name);
		FramebufferData(RenderContext* rc, TextureFactory* tf, uint32_t fboId, const std::string& name);

		void createColorRenderbuffer(uint32_t internalFormat);
		void createDepthRenderbuffer(uint32_t internalFormat);

	private:
		RenderContext* _rc;
		TextureFactory* _textureFactory;
		Texture _renderTargets[MaxRenderTargets];
		Texture _depthBuffer;

		vec2i _size;
		size_t _numTargets;
		
		uint32_t _id;
		uint32_t _colorRenderbuffer;
		uint32_t _depthRenderbuffer;

		bool _isCubemapBuffer;
	};

}