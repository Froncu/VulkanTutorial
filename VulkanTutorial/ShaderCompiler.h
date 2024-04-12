#pragma once

#include <shaderc/shaderc.hpp>
#include <xstring>

namespace vul
{
	class ShaderCompiler final
	{
	public:
		ShaderCompiler(std::string_view shadersDirectory);
		ShaderCompiler(const ShaderCompiler&) = default;
		ShaderCompiler(ShaderCompiler&&) noexcept = default;

		~ShaderCompiler() = default;

		ShaderCompiler& operator=(const ShaderCompiler&) = default;
		ShaderCompiler& operator=(ShaderCompiler&&) noexcept = default;

		[[nodiscard("compiled shader bytecode ignored!")]]
		std::vector<std::uint32_t> operator()(std::string_view shaderFileName, shaderc_shader_kind shaderKind);

		void setShadersDirectory(std::string_view shadersDirectory);

	private:
		shaderc::Compiler m_Compiler{};
		std::string_view m_ShadersDirectory;
	};
}