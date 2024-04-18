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
		void render();

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
		std::unique_ptr<VkPipelineLayout_T, std::function<void(VkPipelineLayout_T*)>> const m_pPipelineLayout;
		std::unique_ptr<VkRenderPass_T, std::function<void(VkRenderPass_T*)>> const m_pRenderPass;
		std::unique_ptr<VkPipeline_T, std::function<void(VkPipeline_T*)>> const m_pPipeline;
		std::vector<std::unique_ptr<VkFramebuffer_T, std::function<void(VkFramebuffer_T*)>>> const m_vpSwapChainFrameBuffers;
		std::unique_ptr<VkCommandPool_T, std::function<void(VkCommandPool_T*)>> const m_pCommandPool;
		std::uint32_t const m_FramesInFlight;
		std::vector<VkCommandBuffer> const m_vCommandBuffers;
		std::vector<std::unique_ptr<VkSemaphore_T, std::function<void(VkSemaphore_T*)>>> const m_vpImageAvailableSemaphores;
		std::vector<std::unique_ptr<VkSemaphore_T, std::function<void(VkSemaphore_T*)>>> const m_vpRenderFinishedSemaphores;
		std::vector<std::unique_ptr<VkFence_T, std::function<void(VkFence_T*)>>> const m_vpInFlightFences;
		uint32_t m_CurrentFrame;
	};
}