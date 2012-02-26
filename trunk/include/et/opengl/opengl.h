#pragma once

#include <iostream>
#include <et/core/debug.h>
#include <et/core/tools.h>

namespace et
{

#if ET_PLATFORM_WIN

	#include <et/opengl/gl.win.h> 
	#define MAX_TEXTURE_UNITS		16

	#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG		0
	#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG		0
	#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG		0
	#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG		0

	#define ET_OPENGLES								0

#elif ET_PLATFORM_IOS

	#include <OpenGLES/ES2/gl.h>
	#include <OpenGLES/ES2/glext.h>

	#define ET_OPENGLES								1

	#define GL_DEPTH_COMPONENT24					GL_DEPTH_COMPONENT24_OES
	#define GL_HALF_FLOAT							GL_HALF_FLOAT_OES
	
	#define GL_RGB8									GL_RGB
	#define GL_RGBA8								GL_RGBA
	
	#define GL_RGB16F								0
	#define GL_RGBA16F								0
	#define GL_RGB32F								0
	#define GL_RGBA32F								0
	#define GL_R									0
	#define GL_RG									0
	#define GL_R16									0
	#define GL_RG16									0
	#define GL_RGB16								0
	#define GL_RGBA16								0

	#define MAX_TEXTURE_UNITS						8

	#define glGenVertexArrays						glGenVertexArraysOES
	#define	glBindVertexArray						glBindVertexArrayOES
	#define	glIsVertexArray							glIsVertexArrayOES
	
	#if !defined(glDeleteVertexArrays)	
		#define glDeleteVertexArrays				glGenVertexArraysOES
	#endif
	
	#define GL_TEXTURE_MAX_LEVEL					GL_TEXTURE_MAX_LEVEL_APPLE
	#define GL_TEXTURE_1D							0
	
	#define GL_VERTEX_ARRAY_BINDING					GL_VERTEX_ARRAY_BINDING_OES

#endif

	enum BufferDrawType
	{
		BufferDrawType_Static,
		BufferDrawType_Stream,
		BufferDrawType_max
	};

	enum TextureWrap
	{
		TextureWrap_Repeat,
		TextureWrap_ClampToEdge,
		TextureWrap_max
	};

	struct OpenGLCounters
	{
		static size_t primitiveCounter;
		static size_t DIPCounter;
		static size_t bindTextureCounter;
		static size_t bindBufferCounter;
		static size_t bindFramebufferCounter;
		static size_t useProgramCounter;
		static size_t bindVertexArrayObjectCounter;
		static void reset();
	};

#if (ET_DEBUG)
	#define checkOpenGLError(tag) checkOpenGLErrorEx(ET_CALL_FUNCTION, __FILE__, ET_TOCONSTCHAR(__LINE__), tag)
#else
	#define checkOpenGLError(tag)
#endif

	void checkOpenGLErrorEx(const char* caller, const char* sourceFile, const char* lineNumber, const std::string& tag);

	std::string glErrorToString(GLenum error);
	std::string glTexTargetToString(int target);
	std::string glInternalFormatToString(int format);
	std::string glTypeToString(int type);

	size_t primitiveCount(GLenum mode, GLsizei count);

	void etViewport(GLint x, GLint y, GLsizei width, GLsizei height);
	void etDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
	void etDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices, GLint base);
	void etBindTexture(GLenum target, GLint texture);
	void etBindBuffer(GLenum target, GLuint buffer);
	void etBindFramebuffer(GLenum target, GLuint framebuffer);
	void etUseProgram(GLuint program);
	void etBindVertexArray(GLuint arr);

	void etCompressedTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, 
		GLsizei imageSize, const GLvoid * data);
	
	void etTexImage1D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, 
		GLenum type, const GLvoid * pixels);
	
	void etCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, 
		GLint border, GLsizei imageSize, const GLvoid * data);

	void etTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height,
		GLint border, GLenum format, GLenum type, const GLvoid * pixels);

	int textureWrapValue(TextureWrap w);

}