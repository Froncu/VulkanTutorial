#include "VulkanApplication.h"
#include "ShaderCompiler.h"

#include <vld.h>
#include <stdexcept>
#include <iostream>

int main()
{
	try
	{
		vul::ShaderCompiler shaderCompiler{ "Shaders" };
		[[maybe_unused]] auto const vBytecodeVertextShader{ shaderCompiler("hardCodedTriangle.vert", shaderc_shader_kind::shaderc_vertex_shader) };
		[[maybe_unused]] auto const vBytecodeFragmentShader{ shaderCompiler("hardCodedTriangle.frag", shaderc_shader_kind::shaderc_fragment_shader) };

	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
	}

	try
	{
		vul::VulkanApplication().run();
	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
	}
}