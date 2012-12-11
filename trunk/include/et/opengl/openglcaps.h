/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

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

		const std::string& glslVersionString() const
			{ return _glslVersionString; }

		void checkCaps();
		
	private:
		std::string _openGlVersion;
		std::string _glslVersionString;
		std::string _glslVersion;

		OpenGLVersion _version;
		bool _mipmap_generation;
		bool _shaders;
		bool _vertex_attrib_arrays;
		bool _vertex_buffers;
		bool _drawelements_basevertex;
	};
	
	inline OpenGLCapabilites& openGLCapabilites()
		{ return OpenGLCapabilites::instance(); }
}