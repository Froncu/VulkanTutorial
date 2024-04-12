#pragma once

#include <Vulkan/vulkan_core.h>
#include <optional>
#include <vector>
#include <memory>
#include <functional>
#include <xstring>

struct GLFWwindow;

namespace vul
{
	struct QueueFamilyIndices final
	{
		std::optional<std::uint32_t> graphics{};
		std::optional<std::uint32_t> present{};

		bool isComplete() const
		{
			return 
				graphics.has_value() and
				present.has_value();
		}
	};

	struct SwapChainSupportDetails final
	{
		VkSurfaceCapabilitiesKHR capabilities{};
		std::vector<VkSurfaceFormatKHR> vFormats{};
		std::vector<VkPresentModeKHR> vPresentModes{};
	};

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

	[[nodiscard("extension's availability result ignored!")]]
	bool isInstanceExtensionAvailable(std::string_view const extensionName);

	[[nodiscard("validation layer's availability result ignored!")]]
	bool isValidationLayerAvailable(std::string_view const validationLayerName);

	[[nodiscard("extension's availability result ignored!")]]
	bool isPhysicalDeviceExtensionAvailable(std::string_view const physicalDeviceExtensionName, VkPhysicalDevice physicalDevice);

	[[nodiscard("physical device's suitability result ignored!")]]
	bool isPhysicalDeviceSuitable(VkPhysicalDevice const physicalDevice, VkSurfaceKHR const windowSurface, std::vector<std::string_view> const& vPhyicalDeviceExtensionNames);
}