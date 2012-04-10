#pragma once

#include <iostream>
#include <map>

#include <et/rendering/renderstate.h>

namespace et
{

	struct ProgramUniform
	{
		GLenum type;
		GLint location;
	};

	struct ProgramAttrib
	{
		std::string name;
		VertexAttributeUsage usage;
		ProgramAttrib(const std::string& a_name, VertexAttributeUsage a_usage) : name(a_name), usage(a_usage) { }
	};

	typedef std::map<std::string, ProgramUniform> UniformMap;
	typedef std::vector<ProgramAttrib> AttribVector;
	typedef UniformMap::iterator UniformIterator;
	typedef StringList ProgramDefinesList;

	class Camera;
	class ProgramData : public APIObjectData
	{
	public:
		static const std::string emptyShaderSource;

	public:
		ProgramData(RenderState& rs, std::string vertexShader, std::string geometryShader, std::string fragmentShader, const std::string& id = "");
		~ProgramData();

		GLint getUniformLocation(const std::string& uniform);
		GLenum getUniformType(const std::string& uniform);

		UniformIterator findUniform(const std::string& name);

		void validate() const;

		int modelViewMatrixUniformLocation() const 
		{ return _mvm_loc; }

		int mvpMatrixUniformLocation() const
		{ return _mvp_loc; }

		int cameraUniformLocation() const
		{ return _cam_loc; }

		int primaryLightUniformLocation() const
		{ return _l0_loc; }

		int lightProjectionMatrixLocation() const
		{ return _lp_loc; }

		void setModelViewMatrix(const mat4 &m);
		void setMVPMatrix(const mat4 &m);
		void setCameraPosition(const vec3& p);
		void setPrimaryLightPosition(const vec3& p);
		void setLightProjectionMatrix(const mat4 &m);

		void setCameraProperties(const Camera& cam);

		template <typename T>
		void setUniform(const std::string& name, const T& value, int count = 1);

		template <typename T>
		void setUniform(int location, int type, const T& value, int count = 1);

		inline GLenum glID() const
			{ return _glID; }

	private:
		void buildProgram(const std::string& vertex_source, const std::string& geom_source, const std::string& frag_source);
		int link();

	private:
		RenderState& _rs;
		GLuint _glID;
		UniformMap _uniforms;
		AttribVector _attributes;

		int _mvm_loc;
		int _mvp_loc;
		int _cam_loc;
		int _l0_loc;
		int _lp_loc;
		bool _loaded;
	};

	template <typename T>
	void ProgramData::setUniform(const std::string& uniform_name, const T& value, int count)
	{
		UniformIterator i = findUniform(uniform_name);
		if (i != _uniforms.end())
			setUniform(i->second.location, i->second.type, value, count);
	}

	template <typename T>
	void ProgramData::setUniform(int nLoc, int type, const T& value, int count)
	{
		if (nLoc == -1) return;

		const GLint* intPtr = reinterpret_cast<const GLint*>(&value);
		const GLfloat* floatPtr = reinterpret_cast<const GLfloat*>(&value);

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
#if (!ET_OPENGLES)
		case GL_SAMPLER_1D:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_1D_ARRAY:
		case GL_SAMPLER_1D_ARRAY_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_2D_ARRAY:
		case GL_SAMPLER_2D_ARRAY_SHADOW:
		case GL_SAMPLER_2D_MULTISAMPLE:
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_SAMPLER_2D_RECT:
		case GL_SAMPLER_2D_RECT_SHADOW:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_BUFFER:
		case GL_SAMPLER_CUBE_MAP_ARRAY:
		case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:
		case GL_SAMPLER_CUBE_SHADOW : 
#endif		 
			{
				glUniform1iv(nLoc, 1, intPtr); 
				break;
			}

		default: 
			{
				std::cout << "ProgramData::setUniform refers to unknown uniform type." << std::endl; 
			}
		}

		checkOpenGLError("ProgramData::setUniform");
	}

}