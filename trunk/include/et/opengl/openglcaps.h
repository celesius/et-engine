#pragma once

#include <et/core/singleton.h>
#include <et/opengl/opengl.h>

namespace et
{
	
	enum OpenGLVersion
	{
		OpenGLVersion_unknown,
		OpenGLVersion_Old,
		OpenGLVersion_New,
		OpenGLVersion_max
	};
	
	class OpenGLCapabilites : public Singleton<OpenGLCapabilites>
	{ 
	public:
		OpenGLCapabilites() : _version(OpenGLVersion_unknown),
			_mipmap_generation(false), _shaders(false), _vertex_attrib_arrays(false), _vertex_buffers(false),
			_drawelements_basevertex(false) { }
		
		bool supportMipMapGeneration() const
			{ return _mipmap_generation; }
		
		bool supportShaders() const 
			{ return _shaders; }
		
		bool supportVertexAttribArrays() const 
			{ return _vertex_attrib_arrays; }
		
		bool supportVertexBuffers() const 
			{ return _vertex_buffers; }
		
		bool supportDrawElemensBaseVertex() const 
			{ return _drawelements_basevertex; }
		
		OpenGLVersion version() const
			{ return _version; }

		const std::string& glslVersion() const
			{ return _glslVersion; }
		
		void checkCaps()
		{
			const char* glv = reinterpret_cast<const char*>(glGetString(GL_VERSION));
			std::string ver(glv ? glv : "<Unknown OpenGL version>");

			char versionHeader[] = "\0\0\0";
			char* ptr = versionHeader;
			std::string version(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
			size_t offset = 0;
			for (std::string::const_iterator i = version.begin(), e = version.end(); i != e; ++i)
			{
				char c = *i;
				if ((c >= '0') && (c <= '9'))
				{
					ptr[offset++] = c;
					if (offset >= 3) break;
				}
				else 
					if (c == ' ') break;
			}
			_glslVersion = versionHeader;
			_version = strToInt(_glslVersion) < 130 ? OpenGLVersion_Old : OpenGLVersion_New;
			
			std::cout << "OpenGL version: " << ver << ", GLSL version: " << _glslVersion << std::endl;

#if (ET_OPENGLES)
			_drawelements_basevertex = false;
#else	   
			_drawelements_basevertex = glDrawElementsBaseVertex != 0;
#endif	   
			_mipmap_generation = glGenerateMipmap != 0;
			_vertex_attrib_arrays = glVertexAttribPointer != 0;
			_vertex_buffers = (glGenBuffers != 0) && (glBindBuffer != 0) && (glBufferData != 0);
			_shaders = (glShaderSource != 0) && (glCreateProgram != 0) && (glCompileShader != 0) && (glLinkProgram != 0);
		};
		
	private:
		std::string _glslVersion;
		OpenGLVersion _version;
		bool _mipmap_generation;
		bool _shaders;
		bool _vertex_attrib_arrays;
		bool _vertex_buffers;
		bool _drawelements_basevertex;
	};
	
	inline OpenGLCapabilites& ogl_caps() { return OpenGLCapabilites::instance(); }
	
}