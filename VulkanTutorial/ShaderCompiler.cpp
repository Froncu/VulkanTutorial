#include "ShaderCompiler.h"

#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include <numeric>
#include <stdexcept>
#include <format>
#include <regex>

#pragma region Constructors/Destructor
vul::ShaderCompiler::ShaderCompiler(std::string_view shadersDirectory)
	: m_ShadersDirectory{ shadersDirectory }
{
}
#pragma endregion Constructors/Destructor



#pragma region Operators
std::vector<std::uint32_t> vul::ShaderCompiler::operator()(std::string_view shaderFileName, shaderc_shader_kind shaderKind)
{
	std::string const fullFileDirectory{ std::format("{}/{}", m_ShadersDirectory, shaderFileName) };

	if (not std::filesystem::exists(fullFileDirectory))
		throw std::runtime_error(std::format("{} does not exist!", fullFileDirectory));

	std::ifstream shaderFile{ fullFileDirectory, std::ifstream::in };
	if (not shaderFile.is_open())
		throw std::runtime_error(std::format("couldn't open {}!", fullFileDirectory));

	std::vector<std::string> vShaderSourceCodeLines{};
	while (not shaderFile.eof())
	{
		std::string lineSourceCode{};
		std::getline(shaderFile, lineSourceCode);

		vShaderSourceCodeLines.emplace_back(lineSourceCode + "\n");
	}

	shaderc::SpvCompilationResult const shaderResult
	{
		m_Compiler.CompileGlslToSpv
		(
			std::reduce(vShaderSourceCodeLines.begin(), vShaderSourceCodeLines.end()),
			shaderKind,
			shaderFileName.data()
		)
	};

	if (shaderResult.GetCompilationStatus() == shaderc_compilation_status_success)
		return { shaderResult.begin(), shaderResult.end() };

	std::string const errorMessage{ shaderResult.GetErrorMessage() };
	std::smatch lineErrorMatch;
	std::regex const regex{ R"(.+:(\d+):.+:.+:  (.+))" };
	if (not std::regex_search(errorMessage, lineErrorMatch, regex))
		throw std::runtime_error("regex_search() failed!");

	int lineNumber{ 1 };
	for (std::string& shaderSourceCodeLine : vShaderSourceCodeLines)
	{
		shaderSourceCodeLine.insert(0, std::format("{}.\t", lineNumber));

		if (lineNumber == std::stoi(lineErrorMatch[1].str()))
			shaderSourceCodeLine.insert(shaderSourceCodeLine.size() - 1, std::format("\t ---> {}", lineErrorMatch[2].str()));

		++lineNumber;
	}

	throw std::runtime_error
	(
		std::format
		(
			"compilation of {} at line {} failed!\n{}",
			fullFileDirectory, lineErrorMatch[1].str(),
			std::reduce(vShaderSourceCodeLines.begin(), vShaderSourceCodeLines.end())
		)
	);
}
#pragma endregion Operators



#pragma region PublicMethods
void vul::ShaderCompiler::setShadersDirectory(std::string_view shadersDirectory)
{
	m_ShadersDirectory = shadersDirectory;
}
#pragma endregion PublicMethods