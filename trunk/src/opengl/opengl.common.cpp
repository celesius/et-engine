#include <et/opengl/opengl.h>

namespace et
{
	static const std::string _before_ = "Before ";

	size_t OpenGLCounters::primitiveCounter = 0;
	size_t OpenGLCounters::DIPCounter = 0;
	size_t OpenGLCounters::bindTextureCounter = 0;
	size_t OpenGLCounters::bindBufferCounter = 0;
	size_t OpenGLCounters::bindFramebufferCounter = 0;
	size_t OpenGLCounters::useProgramCounter = 0;
	size_t OpenGLCounters::bindVertexArrayObjectCounter = 0;

	void OpenGLCounters::reset()
	{
		primitiveCounter = 0;
		DIPCounter = 0;
		bindTextureCounter = 0;
		bindBufferCounter = 0;
		bindFramebufferCounter = 0;
		useProgramCounter = 0;
		bindVertexArrayObjectCounter = 0;
	}

	std::string glErrorToString(GLenum error)
	{
		switch (error)
		{
		case GL_NO_ERROR:
			return "GL_NO_ERROR"; 

		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM"; 

		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE"; 

		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION"; 

		case GL_OUT_OF_MEMORY: 
			return "GL_OUT_OF_MEMORY"; 

		case GL_INVALID_FRAMEBUFFER_OPERATION: 
			return "GL_INVALID_FRAMEBUFFER_OPERATION";

		default: 
			return "Unknown OpenGL error " + intToStr(error);
		} 
	}

	void checkOpenGLErrorEx(const char* caller, const char* sourceFile, const char* lineNumber, const std::string& tag)
	{
		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			std::string out = std::string(sourceFile) + " (" + lineNumber + ") " + caller + " OpenGL error:\n" +
				glErrorToString(err) + (tag.length() ? " at " + tag + "\n": "\n");
			std::cout << out;
		}
	}

	size_t primitiveCount(GLenum mode, GLsizei count)
	{
		switch (mode)
		{
		case GL_TRIANGLES:
			return count / 3;

		case GL_TRIANGLE_STRIP:
			return count - 2;

		case GL_LINES: 
			return count / 2;

		case GL_LINE_STRIP: 
			return count - 1; 

		case GL_POINTS: 
			return count;

		default: 
			return 0;
		};
	}

	std::string glTexTargetToString(int target)
	{
		switch (target)
		{
		case GL_TEXTURE_2D: 
			return "GL_TEXTURE_2D";

		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
			return "GL_TEXTURE_CUBE_MAP_POSITIVE_X";

		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
			return "GL_TEXTURE_CUBE_MAP_NEGATIVE_X";

		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
			return "GL_TEXTURE_CUBE_MAP_POSITIVE_Y";

		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
			return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Y";

		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
			return "GL_TEXTURE_CUBE_MAP_POSITIVE_Z";

		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
			return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Z";

		case GL_TEXTURE_1D: 
			return "GL_TEXTURE_1D";
				
#if defined(GL_TEXTURE_3D)
		case GL_TEXTURE_3D: 
			return "GL_TEXTURE_3D";
#endif
				
#if defined(GL_TEXTURE_2D_ARRAY)
		case GL_TEXTURE_2D_ARRAY: 
			return "GL_TEXTURE_2D_ARRAY";
#endif	
				
#if defined(GL_TEXTURE_CUBE_MAP_ARRAY)
		case GL_TEXTURE_CUBE_MAP_ARRAY: 
			return "GL_TEXTURE_CUBE_MAP_ARRAY";
#endif
				
#if defined(GL_TEXTURE_RECTANGLE)
		case GL_TEXTURE_RECTANGLE: 
			return "GL_TEXTURE_RECTANGLE";
#endif	

#if defined(GL_TEXTURE_2D_MULTISAMPLE)
		case GL_TEXTURE_2D_MULTISAMPLE: 
			return "TEXTURE_2D_MULTISAMPLE";
#endif	

#if defined(GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
		case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
			return "TEXTURE_2D_MULTISAMPLE_ARRAY";
#endif	

#if defined(GL_TEXTURE_BUFFER)
		case GL_TEXTURE_BUFFER: 
			return "TEXTURE_BUFFER_ARB";
#endif	
		default: 
			return "Unknown texture target " + intToStr(target);
		}
	}	

	std::string glInternalFormatToString(int format)
	{
		switch (format)
		{
		case GL_DEPTH_COMPONENT: 
			return "GL_DEPTH_COMPONENT";

		case GL_DEPTH_COMPONENT16: 
			return "GL_DEPTH_COMPONENT16";

		case GL_DEPTH_COMPONENT24: 
			return "GL_DEPTH_COMPONENT24";

		case GL_BGRA: return "GL_BGRA";
		case GL_RGB5_A1: return "GL_RGB5_A1";
				
#if defined (GL_LUMINANCE)				
		case GL_LUMINANCE: return "GL_LUMINANCE";
#endif			
				
#if defined (GL_LUMINANCE_ALPHA)				
		case GL_LUMINANCE_ALPHA: return "GL_LUMINANCE_ALPHA";
#endif
				
		case GL_RGB: return "GL_RGB";
		case GL_RGBA: return "GL_RGBA";

#if defined(GL_INTENSITY)				
			case GL_INTENSITY: return "GL_INTENSITY";
			case GL_INTENSITY8: return "GL_INTENSITY8";
			case GL_INTENSITY16: return "GL_INTENSITY16";
			case GL_LUMINANCE8: return "GL_LUMINANCE8";
			case GL_LUMINANCE16: return "GL_LUMINANCE16";
#endif				
				
#if (!ET_OPENGLES)
		case GL_RGB8: return "GL_RGB8";
		case GL_RGBA4: return "GL_RGBA4";
		case GL_RGBA8: return "GL_RGBA8";
				
		case GL_RGB16F: return "GL_RGB16F";
		case GL_RGBA16F: return "GL_RGBA16F";
		case GL_RGBA32F: return "GL_RGBA32F";
		case GL_RGB32F: return "GL_RGB32F";
				
		case GL_RGB4: return "GL_RGB4";
		case GL_R11F_G11F_B10F: return "GL_R11F_G11F_B10F_EXT";
				
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT: return "GL_COMPRESSED_RGB_S3TC_DXT1_EXT";
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT: return "GL_COMPRESSED_RGBA_S3TC_DXT1_EXT";
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT: return "GL_COMPRESSED_RGBA_S3TC_DXT3_EXT";
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT: return "GL_COMPRESSED_RGBA_S3TC_DXT5_EXT";
		case GL_COMPRESSED_RG_RGTC2: return "GL_COMPRESSED_RG_RGTC2";
#endif				

		default: 
			return "Unknown texture format #" + intToStr(format);
		}
	}	

	std::string glTypeToString(int type)
	{
		switch (type)
		{
		case GL_UNSIGNED_BYTE: 
			return "GL_UNSIGNED_BYTE";

		case GL_BYTE: 
			return "GL_BYTE";

		case GL_UNSIGNED_SHORT: 
			return "GL_UNSIGNED_SHORT";

		case GL_SHORT: 
			return "GL_SHORT";

		case GL_UNSIGNED_INT: 
			return "GL_UNSIGNED_INT";

		case GL_INT: 
			return "GL_INT";

		case GL_FLOAT: 
			return "GL_FLOAT";

		case GL_HALF_FLOAT: 
			return "GL_HALF_FLOAT";

		default: 
			return "Unknown type " + intToStr(type);
		}

	}	

	void etDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices, GLint base)
	{
		checkOpenGLError("begin");

#if (ET_OPENGLES)
		std::cout << "Call to glDrawElementsBaseVertex(" << mode << ", " << count << ", " << type << ", " << indices << ", " << base << ") in OpenGL ES" << std::endl;
#else		
		glDrawElementsBaseVertex(mode, count, type, indices, base);
#endif

		checkOpenGLError("end");

#if !ET_DISABLE_OPENGL_COUNTERS
		OpenGLCounters::primitiveCounter += primitiveCount(mode, count);
		++OpenGLCounters::DIPCounter;
#endif	
	}

	void etDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
	{
		checkOpenGLError("begin");
		glDrawElements(mode, count, type, indices);
		checkOpenGLError("end");

#if !ET_DISABLE_OPENGL_COUNTERS
		OpenGLCounters::primitiveCounter += primitiveCount(mode, count);
		++OpenGLCounters::DIPCounter;
#endif
	}

	void etBindTexture(GLenum target, GLint texture)
	{
		checkOpenGLError("begin");
		glBindTexture(target, texture);
		checkOpenGLError("end");

#if !ET_DISABLE_OPENGL_COUNTERS
		++OpenGLCounters::bindTextureCounter;
#endif
	}

	void etBindBuffer(GLenum target, GLuint buffer)
	{
		checkOpenGLError("begin");
		glBindBuffer(target, buffer);
		checkOpenGLError("end");

#if !ET_DISABLE_OPENGL_COUNTERS
		++OpenGLCounters::bindBufferCounter;
#endif
	}

	void etBindFramebuffer(GLenum target, GLuint framebuffer)
	{
		checkOpenGLError("begin");
		glBindFramebuffer(target, framebuffer);
		checkOpenGLError("end");

#if !ET_DISABLE_OPENGL_COUNTERS
		++OpenGLCounters::bindFramebufferCounter;
#endif
	}

	void etViewport(GLint x, GLint y, GLsizei width, GLsizei height)
	{
		checkOpenGLError("begin");
		glViewport(x, y, width, height);
		checkOpenGLError("end");
	}

	void etUseProgram(GLuint program)
	{
		checkOpenGLError("begin");
		glUseProgram(program);
		checkOpenGLError("end");

#if !ET_DISABLE_OPENGL_COUNTERS
		++OpenGLCounters::useProgramCounter;
#endif
	}

	void etBindVertexArray(GLuint arr)
	{
		checkOpenGLError("begin");
		glBindVertexArray(arr);
		checkOpenGLError("end");

#if !ET_DISABLE_OPENGL_COUNTERS
		++OpenGLCounters::bindVertexArrayObjectCounter;
#endif
	}	

	int textureWrapValue(TextureWrap w)
	{
		switch (w)
		{
		case TextureWrap_ClampToEdge:
			return GL_CLAMP_TO_EDGE;
		default:
			return GL_REPEAT;
		}
	}

	int textureFiltrationValue(TextureFiltration f)
	{
		switch (f)
		{
		case TextureFiltration_Linear:
			return GL_LINEAR;
		case TextureFiltration_NearestMipMapNearest:
			return GL_NEAREST_MIPMAP_NEAREST;
		case TextureFiltration_NearestMipMapLinear:
			return GL_NEAREST_MIPMAP_LINEAR;
		case TextureFiltration_LinearMipMapNearest:
			return GL_LINEAR_MIPMAP_NEAREST;
		case TextureFiltration_LinearMipMapLinear:
			return GL_LINEAR_MIPMAP_LINEAR;
		default:
			return GL_NEAREST;
		}
	}

	void etCompressedTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, 
		GLsizei imageSize, const GLvoid * data)
	{
#if (!ET_OPENGLES)
		glCompressedTexImage1D(target, level, internalformat, width, border, imageSize, data);
#endif

#if (ET_DEBUG)
		std::string err = "glCompressedTexImage1D(" + glTexTargetToString(target) + ", " + intToStr(level) + ", " + 
			glInternalFormatToString(internalformat) + 	", " + intToStr(width) + ", " + 
			intToStr(border) + ", " +  intToStr(imageSize) + ", *" + intToStr(reinterpret_cast<int>(data)) + ") ";
		checkOpenGLError(err);
#endif
	}

	void etCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, 
		GLint border, GLsizei imageSize, const GLvoid * data)
	{
		glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);

#if (ET_DEBUG)
		std::string err = "glCompressedTexImage2D(" + glTexTargetToString(target) + ", " + intToStr(level) + ", " + 
			glInternalFormatToString(internalformat) + 	", " + intToStr(width) + ", " + intToStr(height) + ", " + 
			intToStr(border) + ", " +  intToStr(imageSize) + ", *" + intToStr(reinterpret_cast<int>(data)) + ") ";
		checkOpenGLError(err);
#endif
	}

	void etTexImage1D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, 
		GLenum type, const GLvoid * pixels)
	{
#if (!ET_OPENGLES)
		glTexImage1D(target, level, internalformat, width, border, format, type, pixels);
#endif

#if (ET_DEBUG)
		std::string err = "glTexImage1D(" + glTexTargetToString(target) + ", " + intToStr(level) + ", " + 
			glInternalFormatToString(internalformat) + 	", " + intToStr(width) + ", " + intToStr(border) + ", " + 
			glInternalFormatToString(format) + ", " + glTypeToString(type) + ", *" + intToStr(reinterpret_cast<int>(pixels)) + ") ";
		checkOpenGLError(err);
#endif
	}

	void etTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height,
		GLint border, GLenum format, GLenum type, const GLvoid * pixels)
	{
		glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);

#if (ET_DEBUG)
		std::string err = "glTexImage2D(" + glTexTargetToString(target) + ", " + intToStr(level) + ", " + 
			glInternalFormatToString(internalformat) + 	", " + intToStr(width) + ", " + intToStr(height) + ", " + intToStr(border) + ", " + 
			glInternalFormatToString(format) + ", " + glTypeToString(type) + ", *" + intToStr(reinterpret_cast<int>(pixels)) + ") ";
		checkOpenGLError(err);
#endif
	}

}