/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/app/application.h>
#include <et/opengl/openglcaps.h>
#include <et/rendering/rendercontext.h>
#include <et/apiobjects/programfactory.h>

using namespace et;

ProgramFactory::ProgramFactory(RenderContext* rc) : APIObjectFactory(rc)
{
#if (ET_OPENGLES)	
	_commonHeader = 
		"#define etLowp		lowp\n"
		"#define etMediump	mediump\n"
		"#define etHighp	highp\n";
#else
	_commonHeader = 
		"#version " + ogl_caps().glslVersion() + "\n"
		"#define etLowp\n"
		"#define etMediump\n"
		"#define etHighp\n";
#endif

	if (ogl_caps().version() == OpenGLVersion_Old)
	{
		_fragShaderHeader = 
			"#define etTexture2D	texture2D\n"
			"#define etTextureCube	textureCube\n"
			"#define etFragmentIn	varying\n"
			"#define etFragmentOut	gl_FragColor\n"
			;

		_vertShaderHeader = 
			"#define etVertexIn		attribute\n"
			"#define etVertexOut	varying\n"
			;
	}
	else
	{
		_fragShaderHeader = 
			"#define etTexture2D	texture\n"
			"#define etTextureCube	texture\n"
			"#define etFragmentIn	in\n"
			"#define etFragmentOut	FragColor\n"
			"out vec4 FragColor;\n"
			;

		_vertShaderHeader = 
			"#define etVertexIn		in\n"
			"#define etVertexOut	out\n"
			;
	}
}

Program ProgramFactory::loadProgram(const std::string& file, const ProgramDefinesList& defines)
{
	std::string filename = application().environment().findFile(file);
	if (!fileExists(filename)) 
	{
		std::cout << "Unable to load program from file: " << file << std::endl;
		return Program();
	}

	ProgramDefinesList resultDefines = defines; 
	std::string vertex_source;
	std::string geometry_source;
	std::string fragment_source;
	std::string s;

	std::ifstream progFile(filename.c_str(), std::ios::binary);

	while (!progFile.eof())
	{
		getline(progFile, s);
		trim(s);

		std::string id = s.substr(0, s.find(':'));
		trim(id); 
		lowercase(id);

		if (id == "vs")
			vertex_source = s.substr(s.find_first_of(':') + 1);

		if (id == "gs") 
			geometry_source = s.substr(s.find_first_of(':') + 1);

		if (id == "fs") 
			fragment_source = s.substr(s.find_first_of(':') + 1);

		if (id == "defines") 
			parseDefinesString(s.substr(s.find_first_of(':') + 1), resultDefines);

	} 
	progFile.close();

	normalizeFilePath(trim(vertex_source));
	normalizeFilePath(trim(geometry_source));
	normalizeFilePath(trim(fragment_source));

	std::string vertex_shader;
	std::string geom_shader = ProgramData::emptyShaderSource;
	std::string frag_shader;
	std::string programFolder = getFilePath(filename);
	std::string fName = programFolder + vertex_source;

	if (!fileExists(fName))
		fName = application().environment().findFile(fName);

	if (!fileExists(fName)) 
		fName = application().environment().findFile(vertex_source);

	bool gl2 = ogl_caps().version() == OpenGLVersion_Old;
	std::string gl2Name;

	if (gl2)
		gl2Name = replaceFileExt(fName, ".gl2." + getFileExt(fName));

	if (gl2 && fileExists(gl2Name))
		vertex_shader = loadTextFile(gl2Name);
	else
		vertex_shader = loadTextFile(fName);

	if ((geometry_source.length()) > 0 && (geometry_source != ProgramData::emptyShaderSource))
	{
		fName = programFolder + geometry_source;
		if (!fileExists(fName))
			fName = application().environment().findFile(fName);

		if (!fileExists(fName))
			fName = application().environment().findFile(geometry_source);

		if (fileExists(fName)) 
			vertex_shader = loadTextFile(fName);
	}

	fName = programFolder + fragment_source;
	if (!fileExists(fName)) 
		fName = application().environment().findFile(fName);

	if (!fileExists(fName))
		fName = application().environment().findFile(fragment_source);

	if (gl2)
		gl2Name = replaceFileExt(fName, ".gl2." + getFileExt(fName));

	if (gl2 && fileExists(gl2Name))
		frag_shader = loadTextFile(gl2Name);
	else
		frag_shader = loadTextFile(fName);

	return genProgram(vertex_shader, geom_shader, frag_shader, defines, getFilePath(filename), file);
}

Program ProgramFactory::loadProgram(const std::string& file, const std::string& defines)
{
	ProgramDefinesList defines_list = parseDefinesString(defines);
	return loadProgram(file, defines_list);
}

Program ProgramFactory::genProgram(std::string& vertexshader, std::string& geometryshader, std::string& fragmentshader, 
	const ProgramDefinesList& defines, const std::string& workFolder, const std::string& id)
{
	parseSourceCode(ShaderType_Vertex, vertexshader, defines, workFolder);
	parseSourceCode(ShaderType_Geometry, geometryshader, defines, workFolder);
	parseSourceCode(ShaderType_Fragment, fragmentshader, defines, workFolder);
	return Program(new ProgramData(renderContext()->renderState(), vertexshader, geometryshader, fragmentshader, id));
}

Program ProgramFactory::genProgram(const std::string& vertexshader, const std::string& geometryshader, const std::string& fragmentshader, 
	const ProgramDefinesList& defines, const std::string& workFolder, const std::string& id)
{
	std::string vs = vertexshader;
	std::string gs = geometryshader;
	std::string fs = fragmentshader;
	return genProgram(vs, gs, fs, defines, workFolder, id);
}

void ProgramFactory::parseSourceCode(ShaderType type, std::string& source, const ProgramDefinesList& defines, const std::string& workFolder)
{
	if ((source.length() == 0) || (source == ProgramData::emptyShaderSource)) return;

	std::string header = _commonHeader;
	if (type == ShaderType_Vertex)
		header += _vertShaderHeader;
	else if (type == ShaderType_Fragment)
		header += _fragShaderHeader;
	
	for (ProgramDefinesList::const_iterator i = defines.begin(), e = defines.end(); i != e; ++i)
		header += "\n#define " + *i;

	source = header + "\n" + source;

	std::string::size_type ip = source.find("#include");

	bool hasIncludes = ip != std::string::npos;
	while (hasIncludes)
	{
		while (ip != std::string::npos)
		{
			std::string before = source.substr(0, ip);
			
			source.erase(0, before.size());
			std::string ifname = source.substr(0, source.find_first_of(char(10)));
			std::string include_name = ifname;
			source.erase(0, ifname.size());
			std::string after = source.substr();
			
			if (ifname.find_first_of('"') != -1)
			{
				ifname.erase(0, ifname.find_first_of('"') + 1);
				ifname.erase(ifname.find_last_of('"'));
			}
			else 
			{
				ifname.erase(0, ifname.find_first_of('<') + 1);
				ifname.erase(ifname.find_last_of('>'));
			}
			
			std::string include = "";
			
			if (fileExists(workFolder + ifname))
			{
				include = loadTextFile(workFolder + ifname);
			}
			else
			{
				ifname = application().environment().findFile(ifname);
				if (fileExists(ifname))
					include = loadTextFile(ifname);
				else
					std::cout << "ERROR: failed to include " << ifname << ", starting from folder `" << workFolder << "`" << std::endl;
			}
			
			source = before + include + after;
			ip = source.find("#include");
		}
		
		hasIncludes = ip != std::string::npos;
	}
	
}

void et::parseDefinesString(std::string defines, ProgramDefinesList& storage, std::string separators)
{
	if (separators.length() == 0)
		separators = ",; \t";

	while (defines.length() > 0)
	{
		std::string::size_type separator_pos = std::string::npos;

		for (ProgramDefinesList::size_type s_i = 0 ; s_i < separators.length(); ++s_i)
		{
			std::string::size_type s = defines.find_first_of(separators[s_i]);
			if (s != std::string::npos)
			{
				separator_pos = s;
				break;
			}
		}

		if (separator_pos == std::string::npos)
		{
			storage.push_back(defines);
			break;
		}
		else
		{
			std::string define = defines.substr(0, separator_pos);
			defines.erase(0, separator_pos + 1);

			if (define.size() > 0)
				storage.push_back(define);
		}
	}
}

ProgramDefinesList et::parseDefinesString(std::string defines, std::string separators)
{ 
	ProgramDefinesList value;
	parseDefinesString(defines, value, separators);
	return value;
}