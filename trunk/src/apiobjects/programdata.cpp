/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <fstream>
#include <et/core/tools.h>
#include <et/app/application.h>
#include <et/camera/camera.h>
#include <et/rendering/rendering.h>
#include <et/apiobjects/programdata.h>

using namespace et;

const std::string ProgramData::emptyShaderSource("none");

ProgramData::ProgramData(RenderState& rs, std::string vertexShader, std::string geometryShader,
	std::string fragmentShader, const std::string& id) : APIObjectData(id), _glID(0), _rs(rs), 
	_mModelViewLocation(-1), _mModelViewProjectionLocation(-1), _vCameraLocation(-1), _vPrimaryLightLocation(-1), 
	_mLightProjectionMatrixLocation(-1), _mTransformLocation(-1), _loaded(false)
{
	buildProgram(vertexShader, geometryShader, fragmentShader);
}

ProgramData::~ProgramData()
{
	if ((_glID != 0) && glIsProgram(_glID))
	{
		glDeleteProgram(_glID);
		checkOpenGLError("glDeleteProgram " + name());
	}

	_rs.programDeleted(_glID);
}

UniformIterator ProgramData::findUniform(const std::string& name)
{
	return _uniforms.find(name);
}

void ProgramData::setModelViewMatrix(const mat4& m)
{
	if (_mModelViewLocation < 0) return;

	glUniformMatrix4fv(_mModelViewLocation, 1, false, m.data());
	checkOpenGLError("glUniformMatrix4fv");
}

void ProgramData::setMVPMatrix(const mat4& m)
{
	if (_mModelViewProjectionLocation < 0) return;

	glUniformMatrix4fv(_mModelViewProjectionLocation, 1, false, m.data());
	checkOpenGLError("glUniformMatrix4fv");
}

void ProgramData::setCameraPosition(const vec3& p)
{                 
	if (_vCameraLocation < 0) return;

	glUniform3fv(_vCameraLocation, 1, p.data());
	checkOpenGLError("SetCameraPosition");
}

void ProgramData::setPrimaryLightPosition(const vec3 &p)
{
	if (_vPrimaryLightLocation < 0) return;

	glUniform3fv(_vPrimaryLightLocation, 1, p.data());
	checkOpenGLError("SetPrimaryLightPosition");
}

GLint ProgramData::getUniformLocation(const std::string& uniform) 
{
	UniformIterator i = findUniform(uniform);
	if (i == _uniforms.end()) return -1;

	return i->second.location;
}

GLenum ProgramData::getUniformType(const std::string& uniform) 
{
	UniformIterator i = findUniform(uniform);
	if (i == _uniforms.end()) return 0;

	return i->second.type;
}

ProgramUniform ProgramData::getUniform(const std::string& uniform)
{
	UniformIterator i = findUniform(uniform);
	if (i == _uniforms.end()) return ProgramUniform();

	return i->second;
}

void ProgramData::setLightProjectionMatrix(const mat4& m)
{
	if (_mLightProjectionMatrixLocation < 0) return;

	glUniformMatrix4fv(_mLightProjectionMatrixLocation, 1, false, m.data());
	checkOpenGLError("glUniformMatrix4fv");
}

void ProgramData::setTransformMatrix(const mat4 &m)
{
	if (_mTransformLocation < 0) return;

	glUniformMatrix4fv(_mTransformLocation, 1, false, m.data());
	checkOpenGLError("glUniformMatrix4fv");
}

void ProgramData::setCameraProperties(const Camera& cam)
{
	setModelViewMatrix(cam.modelViewMatrix());
	setMVPMatrix(cam.modelViewProjectionMatrix());
	setCameraPosition(cam.position());
}

void ProgramData::buildProgram(const std::string& vertex_source, const std::string& geom_source, const std::string& frag_source)
{
#if (ET_OPENGLES)
	if (geom_source.length() && (geom_source != ProgramData::emptyShaderSource))
		std::cout << "ProgramData::buildProgram - geometry shader skipped in OpenGL ES" << std::endl;
#endif	
	
	checkOpenGLError("Ce2Render::buildProgram " + name());

	if ((_glID == 0) || !glIsProgram(_glID))
	{
		_glID = glCreateProgram();
		checkOpenGLError("glCreateProgram " + name());
	}

	GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
	checkOpenGLError("glCreateShader<VERT> " + name());

	GLint nLen = (GLint)vertex_source.size();
	const GLchar* src = vertex_source.c_str();

	glShaderSource(VertexShader, 1, &src, &nLen);
	checkOpenGLError("glShaderSource<VERT> " + name());

	glCompileShader(VertexShader);
	checkOpenGLError("glCompileShader<VERT> " + name());

	int cStatus = 0;
	GLsizei nLogLen = 0;
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &cStatus);
	checkOpenGLError("glGetShaderiv<VERT>" + name() + " compile staus ");

	glGetShaderiv(VertexShader, GL_INFO_LOG_LENGTH, &nLogLen);
	if (!cStatus && (nLogLen > 1))
	{
		DataStorage<GLchar> infoLog(nLogLen, 0);
		glGetShaderInfoLog(VertexShader, nLogLen, &nLogLen, infoLog.data());
		std::cout << "Vertex shader " + name() + " compile report:" << std::endl << infoLog.data() << std::endl;
	}

	if (cStatus)
	{
		glAttachShader(_glID, VertexShader);
		checkOpenGLError("glAttachShader<VERT> " + name());
	} 

	GLuint GeometryShader = 0;

#if defined(GL_GEOMETRY_SHADER)
	if ((geom_source.length() > 0) && (geom_source != ProgramData::emptyShaderSource)) 
	{
		GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		checkOpenGLError("glCreateShader<GEOM> " + name());
		nLen = (GLint)geom_source.size();
		src = geom_source.c_str();
		glShaderSource(GeometryShader, 1, &src, &nLen);
		checkOpenGLError("glShaderSource<GEOM> " + name());

		cStatus = 0;
		nLogLen = 0;
		glCompileShader(GeometryShader);
		checkOpenGLError("glCompileShader<GEOM> " + name());
		glGetShaderiv(GeometryShader, GL_COMPILE_STATUS, &cStatus);
		checkOpenGLError("glGetShaderiv<GEOM> " + name() + " compile staus ");
		glGetShaderiv(GeometryShader, GL_INFO_LOG_LENGTH, &nLogLen);
		if (nLogLen > 1)
		{
			DataStorage<GLchar> infoLog(nLogLen, 0);
			glGetShaderInfoLog(GeometryShader, nLogLen, &nLogLen, infoLog.data());
			std::cout << "Geometry shader " + name() + " compile report:" << std::endl << infoLog.data() << std::endl;
		}
		if (cStatus)
		{
			glAttachShader(_glID, GeometryShader);
			checkOpenGLError("glAttachShader<GEOM> " + name());
		} 
	} 
#endif

	///////////////////////////////////////////////// FRAGMENT
	GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	checkOpenGLError("glCreateShader<FRAG> " + name());

	nLen = (GLint)frag_source.size();
	src  = frag_source.c_str();

	glShaderSource(FragmentShader, 1, &src, &nLen);
	checkOpenGLError("glShaderSource<FRAG> " + name());

	glCompileShader(FragmentShader);
	checkOpenGLError("glCompileShader<FRAG> " + name());

	cStatus = 0;
	nLogLen = 0;
	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &cStatus);
	checkOpenGLError("glGetShaderiv<FRAG>" + name() + " compile staus ");

	glGetShaderiv(FragmentShader, GL_INFO_LOG_LENGTH, &nLogLen);
	if (!cStatus && (nLogLen > 1))
	{
		DataStorage<GLchar> infoLog(nLogLen, 0);
		glGetShaderInfoLog(FragmentShader, nLogLen, &nLogLen, infoLog.data());
		std::cout << "Fragment shader " << name() << " compile report:"  << std::endl << infoLog.data() << std::endl;
	}

	if (cStatus)
	{
		glAttachShader(_glID, FragmentShader);
		checkOpenGLError("glAttachShader<FRAG> " + name());

#if (!ET_OPENGLES)
		if (glBindFragDataLocation)
		{
			glBindFragDataLocation(_glID, 0, "FragColor");
			checkOpenGLError("glBindFragDataLocation<color0> " + name());
			glBindFragDataLocation(_glID, 1, "FragColor1");
			checkOpenGLError("glBindFragDataLocation<color1> " + name());
			glBindFragDataLocation(_glID, 2, "FragColor2");
			checkOpenGLError("glBindFragDataLocation<color2> " + name());
			glBindFragDataLocation(_glID, 3, "FragColor3");
			checkOpenGLError("glBindFragDataLocation<color3> " + name());
			glBindFragDataLocation(_glID, 4, "FragColor4");
			checkOpenGLError("glBindFragDataLocation<color4> " + name());
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
			glGetActiveAttrib(_glID, i, maxNameLength, &nameLength, &attribSize, &attribType, name.binary()); 

			VertexAttributeUsage v = stringToVertexAttribute(name.binary());

			if (v != Usage_Undefined)
				_attributes.push_back(ProgramAttrib(name.binary(), v));
			else
				std::cout << "Undefined vertex attribute " << name.binary() << std::endl;
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
				GLsizei uniformLenght = 0;
				GLint uniformSize = 0;
				StringDataStorage name(maxNameLength, 0);
				ProgramUniform P;
				glGetActiveUniform(_glID, i, maxNameLength, &uniformLenght, &uniformSize, &P.type, name.binary());
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
	checkOpenGLError("glLinkProgram " + name());

	glGetProgramiv(_glID, GL_LINK_STATUS, &cStatus);
	checkOpenGLError("glGetProgramiv<GL_LINK_STATUS> " + name());

	glGetProgramiv(_glID, GL_INFO_LOG_LENGTH, &nLogLen);
	checkOpenGLError("glGetProgramiv<GL_INFO_LOG_LENGTH> " + name());

	if (!cStatus && (nLogLen > 1))
	{
		StringDataStorage infoLog(nLogLen + 1, 0);
		glGetProgramInfoLog(_glID, nLogLen, &nLogLen, infoLog.data());
		checkOpenGLError("glGetProgramInfoLog<LINK> " + name());
		std::cout << "Program " << name() << " link log: " << std::endl << infoLog.data() << std::endl;
	}

	return cStatus;
}

void ProgramData::validate() const
{
}