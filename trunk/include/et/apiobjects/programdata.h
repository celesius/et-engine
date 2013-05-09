/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

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
		
		ProgramAttrib(const std::string& a_name, VertexAttributeUsage a_usage) :
			name(a_name), usage(a_usage) { }
	};

	typedef std::map<std::string, ProgramUniform> UniformMap;
	typedef std::vector<ProgramAttrib> AttribVector;
	typedef StringList ProgramDefinesList;

	class Camera;
	class RenderState;
	
	class ProgramData : public APIObject
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
		void setUniform(const std::string& name, const T& value, int count = 1);

		template <typename T>
		void setUniform(const ProgramUniform& u, const T& value, int count = 1);

		template <typename T>
		void setUniform(int location, int type, const T& value, int count = 1);

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
	};

	template <typename T>
	void ProgramData::setUniform(const std::string& uniform_name, const T& value, int count)
	{
		auto i = findUniform(uniform_name);
		if (i != _uniforms.end())
			setUniform(i->second.location, i->second.type, value, count);
	}

	template <typename T>
	void ProgramData::setUniform(const ProgramUniform& u, const T& value, int count)
	{
		setUniform(u.location, u.type, value, count);
	}

	template <typename T>
	void ProgramData::setUniform(int nLoc, int type, const T& value, int count)
	{
		assert(loaded());
		if (nLoc == -1) return;

		const int* intPtr = reinterpret_cast<const int*>(&value);
		const float* floatPtr = reinterpret_cast<const float*>(&value);

		switch (type)
		{
		case GL_FLOAT: 
			{
				glUniform1fv(nLoc, count, floatPtr); 
				break;
			}

		case GL_FLOAT_VEC2:
			{
				glUniform2fv(nLoc, count, floatPtr); 
				break;
			};

		case GL_FLOAT_VEC3:
			{
				glUniform3fv(nLoc, count, floatPtr); 
				break;
			};

		case GL_FLOAT_VEC4:
			{
				glUniform4fv(nLoc, count, floatPtr); 
				break;
			};

		case GL_INT_VEC2 :
			{
				glUniform2iv(nLoc, count, intPtr); 
				break;
			};

		case GL_INT_VEC3 :
			{
				glUniform3iv(nLoc, count, intPtr);
				break;
			};

		case GL_INT_VEC4 :
			{
				glUniform4iv(nLoc, count, intPtr); 
				break;
			};

		case GL_FLOAT_MAT2:
			{
				glUniformMatrix2fv(nLoc, count, false, floatPtr); 
				break;
			};

		case GL_FLOAT_MAT3:
			{
				glUniformMatrix3fv(nLoc, count, false, floatPtr); 
				break;
			};

		case GL_FLOAT_MAT4:
			{
				glUniformMatrix4fv(nLoc, count, false, floatPtr); 
				break;
			};

		case GL_SAMPLER_2D:
		case GL_SAMPLER_CUBE:
				
#if defined(GL_SAMPLER_1D)
		case GL_SAMPLER_1D:
#endif				
#if defined(GL_SAMPLER_1D_SHADOW)
		case GL_SAMPLER_1D_SHADOW:
#endif				
#if defined(GL_SAMPLER_1D_ARRAY)
		case GL_SAMPLER_1D_ARRAY:
#endif				
#if defined(GL_SAMPLER_1D_ARRAY_SHADOW)
		case GL_SAMPLER_1D_ARRAY_SHADOW:
#endif				
#if defined(GL_SAMPLER_2D_SHADOW)
		case GL_SAMPLER_2D_SHADOW:
#endif				
#if defined(GL_SAMPLER_2D_ARRAY)
		case GL_SAMPLER_2D_ARRAY:
#endif				
#if defined(GL_SAMPLER_2D_ARRAY_SHADOW)
		case GL_SAMPLER_2D_ARRAY_SHADOW:
#endif				
#if defined(GL_SAMPLER_2D_MULTISAMPLE)
		case GL_SAMPLER_2D_MULTISAMPLE:
#endif				
#if defined(GL_SAMPLER_2D_MULTISAMPLE_ARRAY)
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
#endif				
#if defined(GL_SAMPLER_2D_RECT)
		case GL_SAMPLER_2D_RECT:
#endif				
#if defined(GL_SAMPLER_2D_RECT_SHADOW)
		case GL_SAMPLER_2D_RECT_SHADOW:
#endif		
#if defined(GL_SAMPLER_3D)				
		case GL_SAMPLER_3D:
#endif				
#if defined(GL_SAMPLER_BUFFER)
		case GL_SAMPLER_BUFFER:
#endif				
#if defined(GL_SAMPLER_CUBE_MAP_ARRAY)
		case GL_SAMPLER_CUBE_MAP_ARRAY:
#endif				
#if defined(GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW)
		case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:
#endif				
#if defined(GL_SAMPLER_CUBE_SHADOW)
		case GL_SAMPLER_CUBE_SHADOW : 
#endif				
			{
				glUniform1iv(nLoc, 1, intPtr); 
				break;
			}

		default:
			log::warning("ProgramData::setUniform refers to unknown uniform type.");
		}
		
		checkOpenGLError("ProgramData::setUniform : %d", nLoc);
	}

}
