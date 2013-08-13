/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/app/application.h>
#include <et/camera/camera.h>
#include <et/rendering/rendering.h>
#include <et/apiobjects/program.h>

using namespace et;

static const std::string etNoShader = "none";

Program::Program(RenderState& rs) : _glID(0), _rs(rs),
	_mModelViewLocation(-1), _mModelViewProjectionLocation(-1), _vCameraLocation(-1),
	_vPrimaryLightLocation(-1), _mLightProjectionMatrixLocation(-1), _mTransformLocation(-1)
{
}

Program::Program(RenderState& rs, const std::string& vertexShader, const std::string& geometryShader,
	const std::string& fragmentShader, const std::string& objName, const std::string& origin) :
	LoadableObject(objName, origin), _glID(0), _rs(rs), _mModelViewLocation(-1),
	_mModelViewProjectionLocation(-1), _vCameraLocation(-1), _vPrimaryLightLocation(-1),
	_mLightProjectionMatrixLocation(-1), _mTransformLocation(-1)
{
	buildProgram(vertexShader, geometryShader, fragmentShader);
}

Program::~Program()
{
	if ((_glID != 0) && glIsProgram(_glID))
	{
		glDeleteProgram(_glID);
		checkOpenGLError("glDeleteProgram: %s", name().c_str());
	}

	_rs.programDeleted(_glID);
}

UniformMap::const_iterator Program::findUniform(const std::string& name) const
{
	assert(loaded());
	return _uniforms.find(name);
}

int Program::getUniformLocation(const std::string& uniform) const
{
	assert(loaded());

	auto i = findUniform(uniform);
	return (i == _uniforms.end()) ? -1 : i->second.location;
}

uint32_t Program::getUniformType(const std::string& uniform) const
{
	assert(loaded());

	auto i = findUniform(uniform);
	return (i == _uniforms.end()) ? 0 : i->second.type;
}

ProgramUniform Program::getUniform(const std::string& uniform) const
{
	assert(loaded());

	auto i = findUniform(uniform);
	return (i == _uniforms.end()) ? ProgramUniform() : i->second;
}

void Program::setModelViewMatrix(const mat4& m)
{
	setUniform(_mModelViewLocation, GL_FLOAT_MAT4, m);
}

void Program::setMVPMatrix(const mat4& m)
{
	setUniform(_mModelViewProjectionLocation, GL_FLOAT_MAT4, m);
}

void Program::setCameraPosition(const vec3& p)
{
	setUniform(_vCameraLocation, GL_FLOAT_VEC3, p);
}

void Program::setPrimaryLightPosition(const vec3 &p)
{
	setUniform(_vPrimaryLightLocation, GL_FLOAT_VEC3, p);
}

void Program::setLightProjectionMatrix(const mat4& m)
{
	setUniform(_mLightProjectionMatrixLocation, GL_FLOAT_MAT4, m);
}

void Program::setTransformMatrix(const mat4 &m)
{
	setUniform(_mTransformLocation, GL_FLOAT_MAT4, m);
}

void Program::setCameraProperties(const Camera& cam)
{
	assert(loaded());

	setModelViewMatrix(cam.modelViewMatrix());
	setMVPMatrix(cam.modelViewProjectionMatrix());
	setCameraPosition(cam.position());
}

void Program::buildProgram(const std::string& vertex_source, const std::string& geom_source,
	const std::string& frag_source)
{
#if (ET_OPENGLES)
	if (geom_source.length() && (geom_source != etNoShader))
		log::info("[Program] Geometry shader skipped in OpenGL ES");
#endif
	
	checkOpenGLError("[Program] buildProgram - %s", name().c_str());

	if ((_glID == 0) || !glIsProgram(_glID))
	{
		_glID = glCreateProgram();
		checkOpenGLError("glCreateProgram - %s", name().c_str());
	}

	uint32_t VertexShader = glCreateShader(GL_VERTEX_SHADER);
	checkOpenGLError("glCreateShader<VERT> - %s", name().c_str());

	int nLen = static_cast<int>(vertex_source.size());
	const GLchar* src = vertex_source.c_str();

	glShaderSource(VertexShader, 1, &src, &nLen);
	checkOpenGLError("glShaderSource<VERT> - %s", name().c_str());

	glCompileShader(VertexShader);
	checkOpenGLError("glCompileShader<VERT> - %s", name().c_str());

	int cStatus = 0;
	GLsizei nLogLen = 0;
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &cStatus);
	checkOpenGLError("glGetShaderiv<VERT> %s compile staus - %s", name().c_str());

	glGetShaderiv(VertexShader, GL_INFO_LOG_LENGTH, &nLogLen);
	if (!cStatus && (nLogLen > 1))
	{
		DataStorage<GLchar> infoLog(nLogLen, 0);
		glGetShaderInfoLog(VertexShader, nLogLen, &nLogLen, infoLog.data());
		log::error("Vertex shader %s compile report:\n%s", name().c_str(), infoLog.data());
	}

	if (cStatus)
	{
		glAttachShader(_glID, VertexShader);
		checkOpenGLError("glAttachShader<VERT> - %s", name().c_str());
	} 

	uint32_t GeometryShader = 0;

#if defined(GL_GEOMETRY_SHADER)
	if ((geom_source.length() > 0) && (geom_source != etNoShader)) 
	{
		GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		checkOpenGLError("glCreateShader<GEOM> - %s", name().c_str());
		nLen = static_cast<int>(geom_source.size());
		src = geom_source.c_str();
		glShaderSource(GeometryShader, 1, &src, &nLen);
		checkOpenGLError("glShaderSource<GEOM> - %s", name().c_str());

		cStatus = 0;
		nLogLen = 0;
		glCompileShader(GeometryShader);
		checkOpenGLError("glCompileShader<GEOM> - %s", name().c_str());

		glGetShaderiv(GeometryShader, GL_COMPILE_STATUS, &cStatus);
		checkOpenGLError("glGetShaderiv<GEOM> %s compile staus", name().c_str());
		
		glGetShaderiv(GeometryShader, GL_INFO_LOG_LENGTH, &nLogLen);
		if (nLogLen > 1)
		{
			DataStorage<GLchar> infoLog(nLogLen, 0);
			glGetShaderInfoLog(GeometryShader, nLogLen, &nLogLen, infoLog.data());
			log::error("Geometry shader %s compile report:\n%s", name().c_str(), infoLog.data());
		}
		if (cStatus)
		{
			glAttachShader(_glID, GeometryShader);
			checkOpenGLError("glAttachShader<GEOM> - %s", name().c_str());
		} 
	} 
#endif

	///////////////////////////////////////////////// FRAGMENT
	uint32_t FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	checkOpenGLError("glCreateShader<FRAG> - %s", name().c_str());

	nLen = static_cast<int>(frag_source.size());
	src  = frag_source.c_str();

	glShaderSource(FragmentShader, 1, &src, &nLen);
	checkOpenGLError("glShaderSource<FRAG> - %s", name().c_str());

	glCompileShader(FragmentShader);
	checkOpenGLError("glCompileShader<FRAG> - %s", name().c_str());

	cStatus = 0;
	nLogLen = 0;
	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &cStatus);
	checkOpenGLError("glGetShaderiv<FRAG> %s compile staus ", name().c_str());

	glGetShaderiv(FragmentShader, GL_INFO_LOG_LENGTH, &nLogLen);
	if (!cStatus && (nLogLen > 1))
	{
		DataStorage<GLchar> infoLog(nLogLen, 0);
		glGetShaderInfoLog(FragmentShader, nLogLen, &nLogLen, infoLog.data());
		log::error("Fragment shader %s compile report:\n%s", name().c_str(), infoLog.data());
	}

	if (cStatus)
	{
		glAttachShader(_glID, FragmentShader);
		checkOpenGLError("glAttachShader<FRAG> - %s", name().c_str());

#if (!ET_OPENGLES)
		if (&glBindFragDataLocation)
		{
			glBindFragDataLocation(_glID, 0, "FragColor");
			checkOpenGLError("glBindFragDataLocation<color0> - %s", name().c_str());
			glBindFragDataLocation(_glID, 1, "FragColor1");
			checkOpenGLError("glBindFragDataLocation<color1> - %s", name().c_str());
			glBindFragDataLocation(_glID, 2, "FragColor2");
			checkOpenGLError("glBindFragDataLocation<color2> - %s", name().c_str());
			glBindFragDataLocation(_glID, 3, "FragColor3");
			checkOpenGLError("glBindFragDataLocation<color3> - %s", name().c_str());
			glBindFragDataLocation(_glID, 4, "FragColor4");
			checkOpenGLError("glBindFragDataLocation<color4> - %s", name().c_str());
		}
#endif
	} 

	cStatus = link();

	if (cStatus)
	{
		int activeAttribs = 0;
		int maxNameLength = 0;
		glGetProgramiv(_glID, GL_ACTIVE_ATTRIBUTES, &activeAttribs);
		glGetProgramiv(_glID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength);
		for (int i = 0; i < activeAttribs; ++i)
		{ 
			int nameLength = 0;
			int attribSize = 0; 
			uint32_t attribType = 0;
			StringDataStorage name(maxNameLength, 0);
			glGetActiveAttrib(_glID, i, maxNameLength, &nameLength, &attribSize, &attribType, name.data());

			VertexAttributeUsage v = stringToVertexAttribute(name.data());

			if (v != Usage_Undefined)
				_attributes.push_back(ProgramAttrib(name.data(), v));
			else
				log::warning("Undefined vertex attribute: %s", name.data());
		}

		for (auto& i : _attributes)
			glBindAttribLocation(_glID, i.usage, i.name.c_str());

		cStatus = link();
		_rs.bindProgram(_glID, true);

		if (cStatus)
		{
			int activeUniforms = 0;
			_uniforms.clear();
			glGetProgramiv(_glID, GL_ACTIVE_UNIFORMS, &activeUniforms);
			glGetProgramiv(_glID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
			for (int i = 0; i < activeUniforms; i++)
			{
				int uSize = 0;
				GLsizei uLenght = 0;
				StringDataStorage name(maxNameLength, 0);
				ProgramUniform P;
				glGetActiveUniform(_glID, i, maxNameLength, &uLenght, &uSize, &P.type, name.binary());
				P.location = glGetUniformLocation(_glID, name.binary());
				_uniforms[name.binary()] = P;

				if (strcmp(name.binary(), "mModelView") == 0)
					_mModelViewLocation = P.location;

				if (strcmp(name.binary(), "mModelViewProjection") == 0) 
					_mModelViewProjectionLocation = P.location;

				if (strcmp(name.binary(), "vCamera") == 0) 
					_vCameraLocation = P.location;

				if (strcmp(name.binary(), "vPrimaryLight") == 0) 
					_vPrimaryLightLocation = P.location;

				if (strcmp(name.binary(), "mLightProjectionMatrix") == 0) 
					_mLightProjectionMatrixLocation = P.location;

				if (strcmp(name.binary(), "mTransform") == 0)
					_mTransformLocation = P.location;
			}
		}
	}

	if (VertexShader != 0)
	{
		glDetachShader(_glID, VertexShader);
		glDeleteShader(VertexShader);
		checkOpenGLError("Deatch and delete vertex shader");
	}

	if (GeometryShader != 0)
	{
		glDetachShader(_glID, GeometryShader);
		glDeleteShader(GeometryShader);
		checkOpenGLError("Deatch and delete geometry shader");
	}
	
	if (FragmentShader != 0)
	{
		glDetachShader(_glID, FragmentShader);
		glDeleteShader(FragmentShader);
		checkOpenGLError("Deatch and delete fragment shader");
	}

	checkOpenGLError("Program::buildProgram -> end"); 
}

int Program::link()
{
	int cStatus = 0;
	int nLogLen = 0;

	glLinkProgram(_glID);
	checkOpenGLError("glLinkProgram - %s", name().c_str());

	glGetProgramiv(_glID, GL_LINK_STATUS, &cStatus);
	checkOpenGLError("glGetProgramiv<GL_LINK_STATUS> - %s", name().c_str());

	glGetProgramiv(_glID, GL_INFO_LOG_LENGTH, &nLogLen);
	checkOpenGLError("glGetProgramiv<GL_INFO_LOG_LENGTH> - %s", name().c_str());

	if (!cStatus && (nLogLen > 1))
	{
		StringDataStorage infoLog(nLogLen + 1, 0);
		glGetProgramInfoLog(_glID, nLogLen, &nLogLen, infoLog.data());
		checkOpenGLError("glGetProgramInfoLog<LINK> - %s", name().c_str());
		log::error("Program %s link log:\n%s", name().c_str(), infoLog.data());
	}

	return cStatus;
}

void Program::validate() const
{
}

/*
 * Uniform setters
 */

void Program::setUniform(int nLoc, int, const int value)
{
	if (nLoc == -1) return;
	
	assert(loaded());
	glUniform1i(nLoc, value);
	checkOpenGLError("setUniform - int");
}

void Program::setUniform(int nLoc, int, const unsigned int value)
{
	if (nLoc == -1) return;
	
	assert(loaded());
	glUniform1i(nLoc, value);
	checkOpenGLError("setUniform - unsigned int");
}

void Program::setUniform(int nLoc, int, const unsigned long value)
{
	if (nLoc == -1) return;
	
	assert(loaded());
	glUniform1i(nLoc, value);
	checkOpenGLError("setUniform - unsigned long");
}

void Program::setUniform(int nLoc, int type, const float value)
{
	if (nLoc == -1) return;
	
	(void)type;
	assert(type == GL_FLOAT);
	assert(loaded());
	glUniform1f(nLoc, value);
	checkOpenGLError("setUniform - float");
}

void Program::setUniform(int nLoc, int type, const vec2& value)
{
	if (nLoc == -1) return;
	
	(void)type;
	assert(type == GL_FLOAT_VEC2);
	assert(loaded());
	
	if ((_vec2Cache.count(nLoc) == 0) || (_vec2Cache[nLoc] != value))
	{
		_vec2Cache[nLoc] = value;
		glUniform2fv(nLoc, 1, value.data());
	}
	
	checkOpenGLError("setUniform - vec2");
}

void Program::setUniform(int nLoc, int type, const vec3& value)
{
	if (nLoc == -1) return;
	
	(void)type;
	assert(type == GL_FLOAT_VEC3);
	assert(loaded());
	
	if ((_vec3Cache.count(nLoc) == 0) || (_vec3Cache[nLoc] != value))
	{
		_vec3Cache[nLoc] = value;
		glUniform3fv(nLoc, 1, value.data());
	}
	
	checkOpenGLError("setUniform - vec3");
}

void Program::setUniform(int nLoc, int type, const vec4& value)
{
	if (nLoc == -1) return;
	
	(void)type;
	assert(type == GL_FLOAT_VEC4);
	assert(loaded());
	
	if ((_vec4Cache.count(nLoc) == 0) || (_vec4Cache[nLoc] != value))
	{
		_vec4Cache[nLoc] = value;
		glUniform4fv(nLoc, 1, value.data());
	}
	
	checkOpenGLError("setUniform - vec4");
}

void Program::setUniform(int nLoc, int type, const mat3& value)
{
	if (nLoc == -1) return;
	
	(void)type;
	assert(type == GL_FLOAT_MAT3);
	assert(loaded());
	
	if ((_mat3Cache.count(nLoc) == 0) || (_mat3Cache[nLoc] != value))
	{
		_mat3Cache[nLoc] = value;
		glUniformMatrix3fv(nLoc, 1, 0, value.data());
	}
	
	checkOpenGLError("setUniform - mat3");
}

void Program::setUniform(int nLoc, int type, const mat4& value)
{
	if (nLoc == -1) return;
	
	(void)type;
	assert(type == GL_FLOAT_MAT4);
	assert(loaded());
	
	if ((_mat4Cache.count(nLoc) == 0) || (_mat4Cache[nLoc] != value))
	{
		_mat4Cache[nLoc] = value;
		glUniformMatrix4fv(nLoc, 1, 0, value.data());
	}
	
	checkOpenGLError("setUniform - mat4");
}
