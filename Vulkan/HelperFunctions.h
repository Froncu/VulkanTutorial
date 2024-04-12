#pragma once

#include <Vulkan/vulkan_core.h>
#include <optional>
#include <vector>
#include <xstring>

struct GLFWwindow;

namespace vul
{
	struct QueueFamilyIndices final
	{
		std::optional<std::uint32_t> graphics{};

		bool isComplete() const
		{
			return graphics.has_value();
		}
	};

	[[nodiscard("handle to created window ignored!")]]
	GLFWwindow* createWindow(int const width, int const height, std::string_view const title);

	[[nodiscard("handle to created instance ignored!")]]
	VkInstance createInstance();

	[[nodiscard("handle to suited physical device ignored!")]]
	VkPhysicalDevice pickSuitedPhysicalDevice(VkInstance const instance);

	[[nodiscard("handle to logical device ignored!")]]
	VkDevice createLogicalDevice(VkPhysicalDevice const physicalDevice);

	[[nodiscard("handle to queue ignored!")]]
	VkQueue getHandleToQueue(VkDevice logicalDevice, std::uint32_t queueFamilyIndex, std::uint32_t queueIndex);

	[[nodiscard("returned available extensions ignored!")]]
	std::vector<VkExtensionProperties> getAvailableExtensions();

	[[nodiscard("returned available validation layers ignored!")]]
	std::vector<VkLayerProperties> getAvailableValidationLayers();

	[[nodiscard("returned available physical devices ignored!")]]
	std::vector<VkPhysicalDevice> getAvailablePhysicalDevices(VkInstance const instance);

	[[nodiscard("returned available queue families ignored!")]]
	std::vector<VkQueueFamilyProperties> getAvailableQueueFamilies(VkPhysicalDevice const physicalDevice);

	[[nodiscard("returned available queue families' indices ignored!")]]
	QueueFamilyIndices getAvailableQueueFamiliesIndices(VkPhysicalDevice const physicalDevice);

	[[nodiscard("extension's availability result ignored!")]]
	bool isExtensionAvailable(std::string_view const extensionName);

	[[nodiscard("validation layer's availability result ignored!")]]
	bool isValidationLayerAvailable(std::string_view const validationLayerName);

	[[nodiscard("physical device's suitability result ignored!")]]
	bool isPhysicalDeviceSuitable(VkPhysicalDevice const physicalDevice);
}