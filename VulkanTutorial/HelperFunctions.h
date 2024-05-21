#pragma once

#include "HelperStructs.h"
#include <memory>
#include <functional>
#include <xstring>

struct GLFWwindow;

namespace vul
{
	[[nodiscard("handle to created window ignored!")]]
	GLFWwindow* createWindow(int const width, int const height, std::string_view const title);

	[[nodiscard("handle to created instance ignored!")]]
	VkInstance createInstance();

	[[nodiscard("handle to window surface ignored!")]]
	VkSurfaceKHR createWindowSurface(VkInstance const instance, GLFWwindow* const pWindow);

	[[nodiscard("handle to suited physical device ignored!")]]
	VkPhysicalDevice pickSuitedPhysicalDevice(VkInstance const instance, VkSurfaceKHR const windowSurface, std::vector<std::string_view> const& vPhyicalDeviceExtensionNames);

	[[nodiscard("handle to logical device ignored!")]]
	VkDevice createLogicalDevice(VkPhysicalDevice const physicalDevice, VkSurfaceKHR const windowSurface, std::vector<std::string_view> const& vPhyicalDeviceExtensionNames);

	[[nodiscard("handle to queue ignored!")]]
	VkQueue getHandleToQueue(VkDevice const logicalDevice, std::uint32_t const queueFamilyIndex, std::uint32_t const queueIndex);

	[[nodiscard("handle to swap chain ignored!")]]
	VkSwapchainKHR createSwapChain(GLFWwindow* const pWindow, VkPhysicalDevice const physicalDevice, VkSurfaceKHR const windowSurface, VkDevice const logicalDevice, VkFormat& swapChainImageFormat, VkExtent2D& swapChainImageExtent);

	[[nodiscard("created swap chain image views ignored!")]]
	std::vector<std::unique_ptr<VkImageView_T, std::function<void(VkImageView_T*)>>> createSwapChainImageViews(std::vector<VkImage> const& vSwapChainImages, VkFormat const swapChainImageFormat, VkDevice const logicalDevice);

	[[nodiscard("handle to shader module ignored!")]]
	VkShaderModule createShaderModule(std::vector<std::uint32_t> const& vBytecode, VkDevice const logicalDevice);

	[[nodiscard("handle to render pass ignored!")]]
	VkRenderPass createRenderPass(VkFormat const swapChainImageFormat, VkDevice const logicalDevice);

	[[nodiscard("handle to pipeline layout ignored!")]]
	VkPipelineLayout createPipelineLayout(VkDevice const logicalDevice);

	[[nodiscard("handle to pipeline ignored!")]]
	VkPipeline createPipeline(VkDevice const logicalDevice, VkExtent2D const swapChainExtent, VkPipelineLayout const pipelineLayout, VkRenderPass const renderPass);

	[[nodiscard("created framebuffers ignored!")]]
	std::vector<std::unique_ptr<VkFramebuffer_T, std::function<void(VkFramebuffer_T*)>>> createFramebuffers(std::vector<std::unique_ptr<VkImageView_T, std::function<void(VkImageView_T*)>>> const& vSwapChainImageViews, VkRenderPass const renderPass, VkExtent2D const swapChainExtent, VkDevice const logicalDevice);

	[[nodiscard("handle to command pool ignored!")]]
	VkCommandPool createCommandPool(VkPhysicalDevice const physicalDevice, VkSurfaceKHR const surface, VkDevice const logicalDevice);

	[[nodiscard("created command buffers ignored!")]]
	std::vector<VkCommandBuffer> createCommandBuffers(VkCommandPool const commandPool, VkDevice const logicalDevice, std::uint32_t const framesInFlight);

	void recordCommandBuffer(VkCommandBuffer const commandBuffer, std::uint32_t const imageIndex, VkRenderPass const renderPass, std::vector<std::unique_ptr<VkFramebuffer_T, std::function<void(VkFramebuffer_T*)>>> const& vpSwapChainFramebuffers, VkExtent2D const swapChainExtent, VkPipeline const pipeline, VkBuffer const vertexBuffer, std::vector<Vertex> const& vVertices);

	[[nodiscard("created semaphores ignored!")]]
	std::vector<std::unique_ptr<VkSemaphore_T, std::function<void(VkSemaphore_T*)>>> createSemaphores(VkDevice const logicalDevice, std::uint32_t const framesInFlight);

	[[nodiscard("handle to semaphore ignored!")]]
	std::vector<std::unique_ptr<VkFence_T, std::function<void(VkFence_T*)>>> createFences(VkDevice const logicalDevice, std::uint32_t const framesInFlight);

	[[nodiscard("handle to buffer ignored!")]]
	std::pair<std::unique_ptr<VkBuffer_T, std::function<void(VkBuffer_T*)>>, std::unique_ptr<VkDeviceMemory_T, std::function<void(VkDeviceMemory_T*)>>>
		createBuffer(VkDevice const logicalDevice, VkPhysicalDevice const physicalDevice, VkDeviceSize const size, VkBufferUsageFlags const usageFlags, VkMemoryPropertyFlags const properties);

	void copyBuffer(VkBuffer const sourceBuffer, VkBuffer const destinationBuffer, VkDeviceSize const size, VkCommandPool const commandPool, VkDevice const logicalDevice, VkQueue const graphicsQueue);

	[[nodiscard("returned available instance extensions ignored!")]]
	std::vector<VkExtensionProperties> getAvailableInstanceExtensions();

	[[nodiscard("returned available validation layers ignored!")]]
	std::vector<VkLayerProperties> getAvailableValidationLayers();

	[[nodiscard("returned available physical devices ignored!")]]
	std::vector<VkPhysicalDevice> getAvailablePhysicalDevices(VkInstance const instance);

	[[nodiscard("returned available physical device extensions ignored!")]]
	std::vector<VkExtensionProperties> getAvailablePhysicalDeviceExtensions(VkPhysicalDevice const physicalDevice);

	[[nodiscard("returned available queue families ignored!")]]
	std::vector<VkQueueFamilyProperties> getAvailableQueueFamilies(VkPhysicalDevice const physicalDevice);

	[[nodiscard("returned available physical device's window surface formats ignored!")]]
	std::vector<VkSurfaceFormatKHR> getAvailablePhysicalDeviceWindowSurfaceFormats(VkPhysicalDevice const physicalDevice, VkSurfaceKHR const windowSurface);

	[[nodiscard("returned available physical device's window surface formats ignored!")]]
	std::vector<VkPresentModeKHR> getAvailablePhysicalDeviceWindowSurfacePresentModes(VkPhysicalDevice const physicalDevice, VkSurfaceKHR const windowSurface);

	[[nodiscard("returned swap chain images ignored!")]]
	std::vector<VkImage> getSwapChainImages(VkDevice const logicalDevice, VkSwapchainKHR const swapChain);

	[[nodiscard("returned available queue families' indices ignored!")]]
	QueueFamilyIndices getAvailableQueueFamiliesIndices(VkPhysicalDevice const physicalDevice, VkSurfaceKHR const windowSurface);

	[[nodiscard("returned swap chain support details ignored!")]]
	SwapChainSupportDetails getSwapChainSupportDetails(VkPhysicalDevice const physicalDevice, VkSurfaceKHR const windowSurface);

	[[nodiscard("returned memory type ignored!")]]
	std::uint32_t getMemoryType(std::uint32_t const typeFilter, VkMemoryPropertyFlags const memoryProperties, VkPhysicalDevice const physicalDevice);

	[[nodiscard("extension's availability result ignored!")]]
	bool isInstanceExtensionAvailable(std::string_view const extensionName);

	[[nodiscard("validation layer's availability result ignored!")]]
	bool isValidationLayerAvailable(std::string_view const validationLayerName);

	[[nodiscard("extension's availability result ignored!")]]
	bool isPhysicalDeviceExtensionAvailable(std::string_view const physicalDeviceExtensionName, VkPhysicalDevice physicalDevice);

	[[nodiscard("physical device's suitability result ignored!")]]
	bool isPhysicalDeviceSuitable(VkPhysicalDevice const physicalDevice, VkSurfaceKHR const windowSurface, std::vector<std::string_view> const& vPhyicalDeviceExtensionNames);
}