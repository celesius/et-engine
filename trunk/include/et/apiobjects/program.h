/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <map>
#include <et/core/object.h>

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
	
	class Program : public LoadableObject
	{
	public:
		ET_DECLARE_POINTER(Program)

	public:
		Program(RenderState& rs);
		
		Program(RenderState& rs, const std::string& vertexShader, const std::string& geometryShader,
			const std::string& fragmentShader, const std::string& objName, const std::string& origin);

		~Program();

		int getUniformLocation(const std::string& uniform);
		uint32_t getUniformType(const std::string& uniform);
		ProgramUniform getUniform(const std::string& uniform);

		void validate() const;

		uint32_t glID() const
			{ return _glID; }
		
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

		void setUniform(int nLoc, int, const int value);
		void setUniform(int nLoc, int, const unsigned int value);
		void setUniform(int nLoc, int, const unsigned long value);
		void setUniform(int nLoc, int type, const float value);
		void setUniform(int nLoc, int type, const vec2& value);
		void setUniform(int nLoc, int type, const vec3& value);
		void setUniform(int nLoc, int type, const vec4& value);
		void setUniform(int nLoc, int type, const mat3& value);
		void setUniform(int nLoc, int type, const mat4& value);
		
		template <typename T>
		void setUniform(const std::string& name, const T& value)
		{
			auto i = findUniform(name);
			if (i != _uniforms.end())
				setUniform(i->second.location, i->second.type, value);
		}

		template <typename T>
		void setUniform(const ProgramUniform& u, const T& value)
			{ setUniform(u.location, u.type, value); }

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
}
