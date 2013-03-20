/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/app/application.h>
#include <et/camera/camera.h>
#include <et/rendering/rendering.h>
#include <et/apiobjects/programdata.h>

using namespace et;

const std::string ProgramData::emptyShaderSource("none");

ProgramData::ProgramData(RenderState& rs) : APIObjectData(std::string()), _glID(0), _rs(rs),
	_mModelViewLocation(-1), _mModelViewProjectionLocation(-1), _vCameraLocation(-1),
	_vPrimaryLightLocation(-1), _mLightProjectionMatrixLocation(-1), _mTransformLocation(-1)
{
}

ProgramData::ProgramData(RenderState& rs, const std::string& vertexShader, const std::string& geometryShader,
	const std::string& fragmentShader, const std::string& id) : APIObjectData(id), _glID(0), _rs(rs),
	_mModelViewLocation(-1), _mModelViewProjectionLocation(-1), _vCameraLocation(-1),
	_vPrimaryLightLocation(-1), _mLightProjectionMatrixLocation(-1), _mTransformLocation(-1)
{
	buildProgram(vertexShader, geometryShader, fragmentShader);
}

ProgramData::~ProgramData()
{
	if ((_glID != 0) && glIsProgram(_glID))
	{
		glDeleteProgram(_glID);
		checkOpenGLError("glDeleteProgram: %s", name().c_str());
	}

	_rs.programDeleted(_glID);
}

UniformIterator ProgramData::findUniform(const std::string& name)
{
	assert(loaded());

	return _uniforms.find(name);
}

void ProgramData::setModelViewMatrix(const mat4& m)
{
	assert(loaded());

	if (_mModelViewLocation < 0) return;

	glUniformMatrix4fv(_mModelViewLocation, 1, false, m.data());
	checkOpenGLError("glUniformMatrix4fv");
}

void ProgramData::setMVPMatrix(const mat4& m)
{
	assert(loaded());

	if (_mModelViewProjectionLocation < 0) return;

	glUniformMatrix4fv(_mModelViewProjectionLocation, 1, false, m.data());
	checkOpenGLError("glUniformMatrix4fv");
}

void ProgramData::setCameraPosition(const vec3& p)
{                 
	assert(loaded());

	if (_vCameraLocation < 0) return;

	glUniform3fv(_vCameraLocation, 1, p.data());
	checkOpenGLError("SetCameraPosition");
}

void ProgramData::setPrimaryLightPosition(const vec3 &p)
{
	assert(loaded());

	if (_vPrimaryLightLocation < 0) return;

	glUniform3fv(_vPrimaryLightLocation, 1, p.data());
	checkOpenGLError("SetPrimaryLightPosition");
}

GLint ProgramData::getUniformLocation(const std::string& uniform) 
{
	assert(loaded());

	UniformIterator i = findUniform(uniform);
	if (i == _uniforms.end()) return -1;

	return i->second.location;
}

GLenum ProgramData::getUniformType(const std::string& uniform) 
{
	assert(loaded());

	UniformIterator i = findUniform(uniform);
	if (i == _uniforms.end()) return 0;

	return i->second.type;
}

ProgramUniform ProgramData::getUniform(const std::string& uniform)
{
	assert(loaded());

	UniformIterator i = findUniform(uniform);
	if (i == _uniforms.end()) return ProgramUniform();

	return i->second;
}

void ProgramData::setLightProjectionMatrix(const mat4& m)
{
	assert(loaded());

	if (_mLightProjectionMatrixLocation < 0) return;

	glUniformMatrix4fv(_mLightProjectionMatrixLocation, 1, false, m.data());
	checkOpenGLError("glUniformMatrix4fv");
}

void ProgramData::setTransformMatrix(const mat4 &m)
{
	assert(loaded());

	if (_mTransformLocation < 0) return;

	glUniformMatrix4fv(_mTransformLocation, 1, false, m.data());
	checkOpenGLError("glUniformMatrix4fv");
}

void ProgramData::setCameraProperties(const Camera& cam)
{
	assert(loaded());

	setModelViewMatrix(cam.modelViewMatrix());
	setMVPMatrix(cam.modelViewProjectionMatrix());
	setCameraPosition(cam.position());
}

void ProgramData::buildProgram(const std::string& vertex_source, const std::string& geom_source,
	const std::string& frag_source)
{
#if (ET_OPENGLES)
	if (geom_source.length() && (geom_source != ProgramData::emptyShaderSource))
		log::info("ProgramData::buildProgram - geometry shader skipped in OpenGL ES");
#endif
	
	checkOpenGLError("Ce2Render::buildProgram - %s", name().c_str());

	if ((_glID == 0) || !glIsProgram(_glID))
	{
		_glID = glCreateProgram();
		checkOpenGLError("glCreateProgram - %s", name().c_str());
	}

	GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
	checkOpenGLError("glCreateShader<VERT> - %s", name().c_str());

	GLint nLen = (GLint)vertex_source.size();
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

	GLuint GeometryShader = 0;

#if defined(GL_GEOMETRY_SHADER)
	if ((geom_source.length() > 0) && (geom_source != ProgramData::emptyShaderSource)) 
	{
		GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		checkOpenGLError("glCreateShader<GEOM> - %s", name().c_str());
		nLen = (GLint)geom_source.size();
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
	GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	checkOpenGLError("glCreateShader<FRAG> - %s", name().c_str());

	nLen = (GLint)frag_source.size();
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
		if (glBindFragDataLocation)
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
		GLint activeAttribs = 0;
		GLint maxNameLength = 0;
		glGetProgramiv(_glID, GL_ACTIVE_ATTRIBUTES, &activeAttribs);
		glGetProgramiv(_glID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength);
		for (GLint i = 0; i < activeAttribs; ++i)
		{ 
			GLint nameLength = 0;
			GLint attribSize = 0; 
			GLenum attribType = 0;
			StringDataStorage name(maxNameLength, 0);
			glGetActiveAttrib(_glID, i, maxNameLength, &nameLength, &attribSize, &attribType, name.data());

			VertexAttributeUsage v = stringToVertexAttribute(name.data());

			if (v != Usage_Undefined)
				_attributes.push_back(ProgramAttrib(name.data(), v));
			else
				log::warning("Undefined vertex attribute: %s", name.data());
		}

		for (AttribVector::iterator i = _attributes.begin(), e = _attributes.end(); i != e; ++i)
			glBindAttribLocation(_glID, i->usage, i->name.c_str());

		cStatus = link();
		_rs.bindProgram(_glID, true);

		if (cStatus)
		{
			GLint activeUniforms = 0;
			_uniforms.clear();
			glGetProgramiv(_glID, GL_ACTIVE_UNIFORMS, &activeUniforms);
			glGetProgramiv(_glID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
			for (GLint i = 0; i < activeUniforms; i++)
			{
				GLint uSize = 0;
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
		glDeleteShader(VertexShader);

	if (GeometryShader != 0)
		glDeleteShader(GeometryShader);

	if (FragmentShader != 0)
		glDeleteShader(FragmentShader);

	checkOpenGLError("ProgramData::buildProgram -> end"); 
}

int ProgramData::link()
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

void ProgramData::validate() const
{
}