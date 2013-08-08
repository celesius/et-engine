/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <map>
#include <et/core/object.h>
#include <et/opengl/opengl.h>

namespace et
{
	struct ProgramUniform
	{
		uint32_t type;
		int location;

		ProgramUniform() :
			type(0), location(-1) { }
	};

	struct ProgramAttrib
	{
		std::string name;
		VertexAttributeUsage usage;
		
		ProgramAttrib(const std::string& aName, VertexAttributeUsage aUsage) :
			name(aName), usage(aUsage) { }
	};

	typedef std::map<std::string, ProgramUniform> UniformMap;
	typedef std::vector<ProgramAttrib> AttribVector;
	typedef StringList ProgramDefinesList;

	class Camera;
	class RenderState;
	
	class ProgramData : public LoadableObject
	{
	public:
		static const std::string emptyShaderSource;

	public:
		ProgramData(RenderState& rs);
		
		ProgramData(RenderState& rs, const std::string& vertexShader, const std::string& geometryShader,
			const std::string& fragmentShader, const std::string& objName, const std::string& origin);

		~ProgramData();

		int getUniformLocation(const std::string& uniform);
		uint32_t getUniformType(const std::string& uniform);
		ProgramUniform getUniform(const std::string& uniform);

		void validate() const;

		bool loaded() const
			{ return _glID != 0; }

		int modelViewMatrixUniformLocation() const 
			{ return _mModelViewLocation; }

		int mvpMatrixUniformLocation() const
			{ return _mModelViewProjectionLocation; }

		int cameraUniformLocation() const
			{ return _vCameraLocation; }

		int primaryLightUniformLocation() const
			{ return _vPrimaryLightLocation; }

		int lightProjectionMatrixLocation() const
			{ return _mLightProjectionMatrixLocation; }

		int transformMatrixLocation() const
			{ return _mTransformLocation; }

		void setModelViewMatrix(const mat4 &m);
		void setMVPMatrix(const mat4 &m);
		void setCameraPosition(const vec3& p);
		void setPrimaryLightPosition(const vec3& p);
		void setLightProjectionMatrix(const mat4 &m);
		void setTransformMatrix(const mat4 &m);

		void setCameraProperties(const Camera& cam);

		template <typename T>
		void setUniform(const std::string& name, const T& value);

		template <typename T>
		void setUniform(const ProgramUniform& u, const T& value);

		template <typename T>
		void setUniform(int location, int type, const T& value);

		uint32_t glID() const
			{ return _glID; }

	private:
		UniformMap::iterator findUniform(const std::string& name);
		
		void buildProgram(const std::string& vertex_source, const std::string& geom_source,
			const std::string& frag_source);

		int link();

	private:
		RenderState& _rs;

		uint32_t _glID;
		UniformMap _uniforms;
		AttribVector _attributes;

		int _mModelViewLocation;
		int _mModelViewProjectionLocation;
		int _vCameraLocation;
		int _vPrimaryLightLocation;
		int _mLightProjectionMatrixLocation;
		int _mTransformLocation;

		mat4 _cachedTransformMatrix;
	};

	template <typename T>
	inline void ProgramData::setUniform(const std::string& uniformName, const T& value)
	{
		auto i = findUniform(uniformName);
		if (i != _uniforms.end())
			setUniform(i->second.location, i->second.type, value);
	}

	template <typename T>
	inline void ProgramData::setUniform(const ProgramUniform& u, const T& value)
		{ setUniform(u.location, u.type, value); }

	template <typename T>
	inline void ProgramData::setUniform(int nLoc, int type, const T& value)
	{
		log::error("Unhandled uniform type: %d", type);
		abort();
	}
	
	template <> void
	inline ProgramData::setUniform(int nLoc, int type, const int& value)
	{
		assert(loaded() && (nLoc >= 0));
		glUniform1i(nLoc, value);
		checkOpenGLError("setUniform - int");
	}
	
	template <> void
	inline ProgramData::setUniform(int nLoc, int type, const unsigned int& value)
	{
		assert(loaded() && (nLoc >= 0));
		glUniform1i(nLoc, value);
		checkOpenGLError("setUniform - unsigned int");
	}
	
	template <> void
	inline ProgramData::setUniform(int nLoc, int, const unsigned long& value)
	{
		assert(loaded() && (nLoc >= 0));
		glUniform1i(nLoc, value);
		checkOpenGLError("setUniform - unsigned long");
	}

	template <> void
	inline ProgramData::setUniform(int nLoc, int type, const float& value)
	{
		assert(type == GL_FLOAT);
		assert(loaded() && (nLoc >= 0));
		glUniform1f(nLoc, value);
		checkOpenGLError("setUniform - float");
	}
	
	template <> void
	inline ProgramData::setUniform(int nLoc, int type, const vec2& value)
	{
		assert(type == GL_FLOAT_VEC2);
		assert(loaded() && (nLoc >= 0));
		glUniform2fv(nLoc, 1, value.data());
		checkOpenGLError("setUniform - vec2");
	}
	
	template <> void
	inline ProgramData::setUniform(int nLoc, int type, const vec3& value)
	{
		assert(type == GL_FLOAT_VEC3);
		assert(loaded() && (nLoc >= 0));
		glUniform3fv(nLoc, 1, value.data());
		checkOpenGLError("setUniform - vec3");
	}
	
	template <> void
	inline ProgramData::setUniform(int nLoc, int type, const vec4& value)
	{
		assert(type == GL_FLOAT_VEC4);
		assert(loaded() && (nLoc >= 0));
		glUniform4fv(nLoc, 1, value.data());
		checkOpenGLError("setUniform - vec4");
	}

	template <> void
	inline ProgramData::setUniform(int nLoc, int type, const mat3& value)
	{
		assert(type == GL_FLOAT_MAT3);
		assert(loaded() && (nLoc >= 0));
		glUniformMatrix3fv(nLoc, 1, 0, value.data());
		checkOpenGLError("setUniform - mat3");
	}
	
	template <> void
	inline ProgramData::setUniform(int nLoc, int type, const mat4& value)
	{
		assert(type == GL_FLOAT_MAT4);
		assert(loaded() && (nLoc >= 0));
		glUniformMatrix4fv(nLoc, 1, 0, value.data());
		checkOpenGLError("setUniform - mat4");
	}
}

