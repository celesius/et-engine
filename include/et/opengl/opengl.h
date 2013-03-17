/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/core/et.h>

#if ET_PLATFORM_WIN
#
#	include <et/platform-win/opengl.win.h>
#
#	define ET_OPENGLES								0
#	define ET_OPENGL3_AVAILABLE						1
#	define ET_OPENGL4_AVAILABLE						1
#
#elif ET_PLATFORM_MAC
#
#	include <OpenGL/OpenGL.h>
#	if defined(CGL_VERSION_1_3)
#
#		include <OpenGL/gl3.h>
#		include <OpenGL/gl3ext.h>
#
#		define ET_OPENGL3_AVAILABLE					1
#
#	elif defined(CGL_VERSION_1_2)
#
#		define glBindFragDataLocation				glBindFragDataLocationEXT
#		define glGenVertexArrays					glGenVertexArraysAPPLE
#		define glBindVertexArray					glBindVertexArrayAPPLE
#		define glIsVertexArray						glIsVertexArrayAPPLE
#		define glDeleteVertexArrays					glDeleteVertexArraysAPPLE
#		define GL_VERTEX_ARRAY_BINDING				GL_VERTEX_ARRAY_BINDING_APPLE
#
#		define ET_OPENGL3_AVAILABLE					0
#
#	else
#
#		error Unsupported OpenGL version
#
#	endif
#
#	define ET_OPENGLES								0
#	define ET_OPENGL4_AVAILABLE						0
#
#elif ET_PLATFORM_IOS
#
#	include <OpenGLES/ES2/gl.h>
#	include <OpenGLES/ES2/glext.h>
#
#	define ET_OPENGLES								1
#
#	define GL_DEPTH_COMPONENT24						GL_DEPTH_COMPONENT24_OES
#	define GL_HALF_FLOAT							GL_HALF_FLOAT_OES
#	define GL_RGB8									GL_RGB
#	define GL_RGBA8									GL_RGBA
#
#	define glGenVertexArrays						glGenVertexArraysOES
#	define glBindVertexArray						glBindVertexArrayOES
#	define glIsVertexArray							glIsVertexArrayOES
#	define glDeleteVertexArrays						glDeleteVertexArraysOES
#	define glClearDepth								glClearDepthf
#
#	define ET_OPENGL3_AVAILABLE						1
#	define ET_OPENGL4_AVAILABLE						0
#	define GL_TEXTURE_MAX_LEVEL						GL_TEXTURE_MAX_LEVEL_APPLE
#	define GL_VERTEX_ARRAY_BINDING					GL_VERTEX_ARRAY_BINDING_OES
#
#elif (ET_PLATFORM_ANDROID)
#
#	include <EGL/egl.h>
#	include <GLES2/gl2.h>
#	include <GLES2/gl2ext.h>
#
#	define ET_OPENGLES								1
#	define ET_OPENGL3_AVAILABLE						0
#	define ET_OPENGL4_AVAILABLE						0
#
#	define GL_DEPTH_COMPONENT24						GL_DEPTH_COMPONENT24_OES
#	define GL_HALF_FLOAT							GL_HALF_FLOAT_OES
#	define GL_RGB8									GL_RGB
#	define GL_RGBA8									GL_RGBA
#
#	define glClearDepth								glClearDepthf
#
#else
#
#	error Platform is not defined
#
#endif

#if !defined(glFramebufferTexture)
#
#	define glFramebufferTexture(target, attachment, texture, level) \
			glFramebufferTexture2D(target, attachment, GL_TEXTURE_2D, texture, level)
#
#endif

#if !defined(GL_COLOR_ATTACHMENT0)
#
#	define GL_COLOR_ATTACHMENT0						0
#
#endif

#if !defined(GL_COMPARE_REF_TO_TEXTURE) && defined(GL_COMPARE_R_TO_TEXTURE)
#
#	define GL_COMPARE_REF_TO_TEXTURE				GL_COMPARE_R_TO_TEXTURE
#
#endif

#if (ET_DEBUG)
#
#	define checkOpenGLError(...) \
		checkOpenGLErrorEx(ET_CALL_FUNCTION, __FILE__, ET_TOCONSTCHAR(__LINE__), __VA_ARGS__);
#
#else
#
#	define checkOpenGLError(...)
#
#endif

namespace et
{
	enum BufferDrawType
	{
		BufferDrawType_Static,
		BufferDrawType_Dynamic,
		BufferDrawType_Stream,
		BufferDrawType_max
	};

	enum TextureWrap
	{
		TextureWrap_Repeat,
		TextureWrap_ClampToEdge,
		TextureWrap_MirrorRepeat,
		TextureWrap_max
	};

	enum TextureFiltration
	{
		TextureFiltration_Nearest,
		TextureFiltration_Linear,
		TextureFiltration_NearestMipMapNearest,
		TextureFiltration_LinearMipMapNearest,
		TextureFiltration_NearestMipMapLinear,
		TextureFiltration_LinearMipMapLinear,
		TextureFiltration_max
	};
	
	enum PrimitiveType
	{
		PrimitiveType_Points,
		PrimitiveType_Lines,
		PrimitiveType_Triangles,
		PrimitiveType_TriangleStrips,
		
		PrimitiveType_LineStrip,
		
		PrimitiveType_max
	};
	
	enum
	{
		MaxTextureUnits	= 8
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

	void checkOpenGLErrorEx(const char* caller, const char* fileName, const char* line, const char* tag, ...);

	std::string glErrorToString(GLenum error);
	std::string glTexTargetToString(int target);
	std::string glInternalFormatToString(int format);
	std::string glTypeToString(int type);
	std::string glBlendFuncToString(int value);

	void validateExtensions();

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

	int textureWrapValue(TextureWrap);
	int textureFiltrationValue(TextureFiltration);
	int drawTypeValue(BufferDrawType);
	int primitiveTypeValue(PrimitiveType);
}