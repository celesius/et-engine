/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/core/tools.h>
#include <et/opengl/openglcaps.h>

using namespace et;

OpenGLCapabilites::OpenGLCapabilites() : _version(OpenGLVersion_unknown),
	_mipmap_generation(false), _shaders(false), _vertex_arrays(false),
	_vertex_attrib_arrays(false), _vertex_buffers(false), _drawelements_basevertex(false)
{
}


void OpenGLCapabilites::checkCaps()
{
	const char* glv = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	const char* glslv = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	checkOpenGLError("OpenGLCapabilites::checkCaps");
	
	_glslVersion = std::string();
	_openGlVersion = std::string(glv ? glv : "<Unknown OpenGL version>");
	_glslVersionString = std::string(glslv ? glslv : "<Unknown GLSL version>");

	if (glslv != nullptr)
	{
		do
		{
			if ((_glslVersion.size() > 0) && (*glslv == ET_SPACE)) break;

			if ((*glslv >= '0') && (*glslv <= '9'))
				_glslVersion.push_back(*glslv);
		}
		while (*glslv++);
	}
	
	_version = strToInt(_glslVersion) < 130 ? OpenGLVersion_Old : OpenGLVersion_New;

	log::info("OpenGL version: %s\nGLSL version: %s, (%s)",
		_openGlVersion.c_str(), _glslVersionString.c_str(), _glslVersion.c_str());
	
#if (ET_OPENGL4_AVAILABLE)
	_drawelements_basevertex = glDrawElementsBaseVertex != nullptr;
#else
	_drawelements_basevertex = false;
#endif
	
	_mipmap_generation = glGenerateMipmap != nullptr;
	
#if (ET_OPENGL3_AVAILABLE)
	_vertex_arrays = (glGenVertexArrays != nullptr) && (glDeleteVertexArrays != nullptr)
		&& (glBindVertexArray != nullptr) && (glIsVertexArray != nullptr);
	
	if (_vertex_arrays)
	{
		GLuint testArray = 0;
		glGenVertexArrays(1, &testArray);
		if ((glGetError() == GL_NO_ERROR) && (testArray != 0))
			glDeleteVertexArrays(1, &testArray);
		else
			_vertex_arrays = false;
	}
#else
	_vertex_arrays = false;
#endif

	_vertex_attrib_arrays = glVertexAttribPointer != nullptr;

	_vertex_buffers = (glGenBuffers != nullptr) && (glBindBuffer != nullptr) &&
		(glBufferData != nullptr);

	_shaders = (glShaderSource != nullptr) && (glCreateProgram != nullptr) &&
		(glCompileShader != nullptr) && (glLinkProgram != nullptr);
};
