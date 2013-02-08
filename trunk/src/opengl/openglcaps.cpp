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
	
	std::cout << "OpenGL version: " << _openGlVersion << std::endl <<
				 "GLSL version: " << _glslVersionString << " (" << _glslVersion << ")" << std::endl;
	
#if (ET_OPENGL3_AVAILABLE)
	_drawelements_basevertex = glDrawElementsBaseVertex != nullptr;
#else
	_drawelements_basevertex = false;
#endif
	
	_mipmap_generation = glGenerateMipmap != nullptr;
	
	_vertex_attrib_arrays = glVertexAttribPointer != nullptr;

	_vertex_buffers = (glGenBuffers != nullptr) && (glBindBuffer != nullptr) &&
		(glBufferData != nullptr);

	_shaders = (glShaderSource != nullptr) && (glCreateProgram != nullptr) &&
		(glCompileShader != nullptr) && (glLinkProgram != nullptr);
};
