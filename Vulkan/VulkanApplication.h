#pragma once

#include <Vulkan/vulkan_core.h>
#include <memory>
#include <functional>
#include <optional>
#include <array>
#include <xstring>

struct GLFWwindow;

namespace vul
{
	constexpr int g_WindowWidth{ 800 };
	constexpr int g_WindowHeight{ 600 };
	std::vector<std::string_view> const vPhysicalDeviceExtensionNames{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

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
		std::unique_ptr<VkSurfaceKHR_T, std::function<void(VkSurfaceKHR_T*)>> const m_pWindowSurface;
		VkPhysicalDevice const m_PhysicalDevice;
		std::unique_ptr<VkDevice_T, std::function<void(VkDevice_T*)>> const m_pLogicalDevice;
		VkQueue const m_GraphicsQueue;
		VkQueue const m_PresentQueue;
		std::unique_ptr<VkSwapchainKHR_T, std::function<void(VkSwapchainKHR_T*)>> const m_pSwapChain;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainImageExtent;
		std::vector<VkImage> const m_vSwapChainImages;
		std::vector<std::unique_ptr<VkImageView_T, std::function<void(VkImageView_T*)>>> const m_vpSwapChainImageViews;
	};
}