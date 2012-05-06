/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/apiobjects/program.h>
#include <et/apiobjects/apiobjectfactory.h>

namespace et
{
	enum ShaderType
	{
		ShaderType_Vertex,
		ShaderType_Geometry,
		ShaderType_Fragment,
		ShaderType_max
	};
	
	class RenderContext;
	class ProgramFactory : public APIObjectFactory
	{
	public:
		ProgramFactory(RenderContext* rc);

		Program loadProgram(const std::string& file, const std::string& defines = "");

		Program loadProgram(const std::string& file, const ProgramDefinesList& defines);

		Program genProgram(std::string& vertexshader, std::string& geometryshader, std::string& fragmentshader, 
			const ProgramDefinesList& defines = ProgramDefinesList(0), const std::string& workFolder = ".", 
			const std::string& id = "");

		Program genProgram(const std::string& vertexshader, const std::string& geometryshader, const std::string& fragmentshader, 
			const ProgramDefinesList& defines = ProgramDefinesList(0), const std::string& workFolder = ".", 
			const std::string& id = "");

		void parseSourceCode(ShaderType type, std::string& code, const ProgramDefinesList& defines, const std::string& workFolder = ".");

	private:
		std::string _commonHeader;
		std::string _fragShaderHeader;
		std::string _vertShaderHeader;
	};

	ProgramDefinesList parseDefinesString(std::string defines, std::string separators = ",; \t");
	void parseDefinesString(std::string defines, ProgramDefinesList& storage, std::string separators = ",; \t");

}