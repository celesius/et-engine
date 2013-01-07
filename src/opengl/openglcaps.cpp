/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/core/tools.h>
#include <et/opengl/openglcaps.h>

using namespace et;

void OpenGLCapabilites::checkCaps()
{
	const char* glv = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	const char* glslv = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	_glslVersion = std::string();
	_openGlVersion = std::string(glv ? glv : "<Unknown OpenGL version>");
	_glslVersionString = std::string(glslv ? glslv : "<Unknown GLSL version>");
	
	const char* ptr = glslv;
	do
	{
		if (_glslVersion.size() && (*ptr == ET_SPACE)) break;
		
		if ((*ptr >= '0') && (*ptr <= '9'))
			_glslVersion.push_back(*ptr);
	}
	while (*ptr++);
	
	_version = strToInt(_glslVersion) < 130 ? OpenGLVersion_Old : OpenGLVersion_New;
	
	std::cout << "OpenGL version: " << _openGlVersion << std::endl <<
				 "GLSL version: " << _glslVersionString << " (" << _glslVersion << ")" << std::endl;
	
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
