#pragma once

#include <Vulkan/vulkan_core.h>
#include <memory>
#include <functional>

struct GLFWwindow;

namespace vul
{
	constexpr int g_WindowWidth{ 800 };
	constexpr int g_WindowHeight{ 600 };

	class VulkanApplication final
	{
	public:
		VulkanApplication();
		VulkanApplication(const VulkanApplication&) = delete;
		VulkanApplication(VulkanApplication&&) noexcept = delete;

		~VulkanApplication();

		VulkanApplication& operator=(const VulkanApplication&) = delete;
		VulkanApplication& operator=(VulkanApplication&&) noexcept = delete;

		void run();

	private:
		std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>> const m_pWindow;
		std::unique_ptr<VkInstance_T, std::function<void(VkInstance_T*)>> const m_pInstance;
	};
}