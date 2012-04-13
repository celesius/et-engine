#pragma once

#include <et/core/intrusiveptr.h>
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

		GLint colorInternalformat;
		GLenum colorFormat;
		GLenum colorType;

		GLint depthInternalformat;
		GLenum depthFormat;
		GLenum depthType;
		
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
		static const int MaxRenderTargets = 16;

	public:
		~FramebufferData();

		bool check();

		Texture renderTarget(size_t index = 0) const
			{ return (index < MaxRenderTargets) ? _renderTargets[index] : Texture(); }

		Texture depthBuffer() const
			{ return _depthBuffer; }

		bool addRenderTarget(const Texture& texture);
		void addSameRendertarget();

		bool setDepthTarget(const Texture& texture);
		bool setDepthTarget(const Texture& texture, GLenum target);

		bool setCurrentRenderTarget(const Texture& texture, GLenum target);
		bool setCurrentRenderTarget(const Texture& texture);
		bool setCurrentRenderTarget(GLenum rt);

		bool setCurrentCubemapFace(size_t faceIndex);

		void setDrawBuffersCount(int c);

		inline GLuint glID() const
			{ return _id; }

		inline vec2i size() const
			{ return _size; }
	
		inline GLuint colorRenderbuffer() const
			{ return _colorRenderbuffer; }
		
		inline GLuint depthRenderbuffer() const
			{ return _depthRenderbuffer; }

		inline void setColorRenderbuffer(GLuint r)
			{ _colorRenderbuffer = r; }
		
		inline void setDepthRenderbuffer(GLuint r) 
			{ _depthRenderbuffer = r; }
		
		void forceSize(int w, int h);
		
	private:
		friend class FramebufferFactory;
		FramebufferData(RenderContext* rc, TextureFactory* tf, const FramebufferDescription& desc, const std::string& name);
		FramebufferData(RenderContext* rc, TextureFactory* tf, GLuint fboId, const std::string& name);

		void createColorRenderbuffer(GLenum internalFormat);
		void createDepthRenderbuffer(GLenum internalFormat);

	private:
		RenderContext* _rc;
		TextureFactory* _textureFactory;
		Texture _renderTargets[MaxRenderTargets];
		Texture _depthBuffer;

		GLuint _id;
		vec2i _size;
		GLenum _numTargets;
		
		GLuint _colorRenderbuffer;
		GLuint _depthRenderbuffer;

		bool _isCubemapBuffer;
	};

}