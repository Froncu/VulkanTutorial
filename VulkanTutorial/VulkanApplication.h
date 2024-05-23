#pragma once

#include "HelperStructs.h"
#include "Window.h"

#include <Vulkan/vulkan_core.h>
#include <memory>
#include <functional>
#include <optional>
#include <array>
#include <xstring>

struct GLFWwindow;

namespace fro
{
	constexpr int g_WindowWidth{ 800 };
	constexpr int g_WindowHeight{ 600 };
	std::vector<std::string_view> const vPhysicalDeviceExtensionNames{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	class VulkanApplication final
	{
	public:
		VulkanApplication();

		~VulkanApplication();

		void run();

	private:
		VulkanApplication(const VulkanApplication&) = delete;
		VulkanApplication(VulkanApplication&&) noexcept = delete;

		VulkanApplication& operator=(const VulkanApplication&) = delete;
		VulkanApplication& operator=(VulkanApplication&&) noexcept = delete;

		void render();
		void recreateSwapChain();
		std::pair<std::unique_ptr<VkBuffer_T, std::function<void(VkBuffer_T*)>>, std::unique_ptr<VkDeviceMemory_T, std::function<void(VkDeviceMemory_T*)>>>
		createVertexBuffer();
		std::pair<std::unique_ptr<VkBuffer_T, std::function<void(VkBuffer_T*)>>, std::unique_ptr<VkDeviceMemory_T, std::function<void(VkDeviceMemory_T*)>>>
		createIndexBuffer();
		void createUniformBuffers();
		void updateUniformBuffer();
		void createDescriptorSets();
		void createTextureImage();
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		void createTextureImageView();
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

		Window const m_Window{ "Vulkan", g_WindowWidth, g_WindowHeight };

		std::unique_ptr<VkInstance_T, std::function<void(VkInstance_T*)>> const m_pInstance;
		std::unique_ptr<VkSurfaceKHR_T, std::function<void(VkSurfaceKHR_T*)>> const m_pWindowSurface;
		VkPhysicalDevice const m_PhysicalDevice;
		std::unique_ptr<VkDevice_T, std::function<void(VkDevice_T*)>> const m_pLogicalDevice;
		VkQueue const m_GraphicsQueue;
		VkQueue const m_PresentQueue;
		std::unique_ptr<VkSwapchainKHR_T, std::function<void(VkSwapchainKHR_T*)>> m_pSwapChain;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainImageExtent;
		std::vector<VkImage> m_vSwapChainImages;
		std::vector<std::unique_ptr<VkImageView_T, std::function<void(VkImageView_T*)>>> m_vpSwapChainImageViews;
		std::unique_ptr<VkDescriptorSetLayout_T, std::function<void(VkDescriptorSetLayout_T*)>> const m_pDescriptorSetLayout;
		std::uint32_t const m_FramesInFlight;
		std::unique_ptr<VkDescriptorPool_T, std::function<void(VkDescriptorPool_T*)>> m_pDescriptorPool;
		std::unique_ptr<VkPipelineLayout_T, std::function<void(VkPipelineLayout_T*)>> const m_pPipelineLayout;
		std::unique_ptr<VkRenderPass_T, std::function<void(VkRenderPass_T*)>> const m_pRenderPass;
		std::unique_ptr<VkPipeline_T, std::function<void(VkPipeline_T*)>> const m_pPipeline;
		std::vector<std::unique_ptr<VkFramebuffer_T, std::function<void(VkFramebuffer_T*)>>> m_vpSwapChainFrameBuffers;
		std::unique_ptr<VkCommandPool_T, std::function<void(VkCommandPool_T*)>> const m_pCommandPool;
		std::vector<VkCommandBuffer> const m_vCommandBuffers;
		std::vector<std::unique_ptr<VkSemaphore_T, std::function<void(VkSemaphore_T*)>>> const m_vpImageAvailableSemaphores;
		std::vector<std::unique_ptr<VkSemaphore_T, std::function<void(VkSemaphore_T*)>>> const m_vpRenderFinishedSemaphores;
		std::vector<std::unique_ptr<VkFence_T, std::function<void(VkFence_T*)>>> const m_vpInFlightFences;
		uint32_t m_CurrentFrame;
		bool m_FramebufferResized;
		std::vector<Vertex> const m_vVertices;
		std::vector<std::uint16_t> const m_vIndices;
		std::pair<
			std::unique_ptr<VkBuffer_T, std::function<void(VkBuffer_T*)>>,
			std::unique_ptr<VkDeviceMemory_T, std::function<void(VkDeviceMemory_T*)>>> m_pVertexBuffer;
		std::pair<
			std::unique_ptr<VkBuffer_T, std::function<void(VkBuffer_T*)>>,
			std::unique_ptr<VkDeviceMemory_T, std::function<void(VkDeviceMemory_T*)>>> m_pIndexBuffer;
		std::vector<std::pair<
			std::unique_ptr<VkBuffer_T, std::function<void(VkBuffer_T*)>>,
			std::unique_ptr<VkDeviceMemory_T, std::function<void(VkDeviceMemory_T*)>>>> m_vpUniformBuffers;
		std::vector<void*> m_vUniformBuffersMapped;
		std::vector<VkDescriptorSet> m_vDescriptorSets;
		std::pair<
			std::unique_ptr<VkImage_T, std::function<void(VkImage_T*)>>,
			std::unique_ptr<VkDeviceMemory_T, std::function<void(VkDeviceMemory_T*)>>> m_pTextureImage;
		std::unique_ptr<VkImageView_T, std::function<void(VkImageView_T*)>> m_pTextureImageView;
		std::unique_ptr<VkSampler_T, std::function<void(VkSampler_T*)>> m_pTextureImageSampler;
	};
}