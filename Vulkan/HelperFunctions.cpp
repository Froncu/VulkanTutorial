#include "HelperFunctions.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#undef GLFW_INCLUDE_VULKAN
#include <algorithm>
#include <stdexcept>
#include <format>

#pragma region HelperFunctions
GLFWwindow* vul::createWindow(int const width, int const height, std::string_view const title)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	return glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
}

VkInstance vul::createInstance()
{
#ifndef NDEBUG
	std::vector const vRequiredValidationLayerNames{ "VK_LAYER_KHRONOS_validation" };

	for (std::string_view const requiredValidationLayerName : vRequiredValidationLayerNames)
		if (!isValidationLayerAvailable(requiredValidationLayerName))
			throw std::runtime_error(std::format("validation layer {} is not available!", requiredValidationLayerName));
#endif

	std::uint32_t requiredExtensionCount;
	char const* const* const ppRequiredExtensionNames{ glfwGetRequiredInstanceExtensions(&requiredExtensionCount) };

	std::vector<std::string_view> const vRequiredExtensionNames{ ppRequiredExtensionNames, ppRequiredExtensionNames + requiredExtensionCount };
	for (std::string_view const requiredExtensionName : vRequiredExtensionNames)
		if (!isExtensionAvailable(requiredExtensionName))
			throw std::runtime_error(std::format("extension {} is not available!", requiredExtensionName));

	VkInstanceCreateInfo const instanceCreateInfo
	{
		.sType{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO },
#ifndef NDEBUG
		.enabledLayerCount{ static_cast<uint32_t>(vRequiredValidationLayerNames.size()) },
		.ppEnabledLayerNames{ vRequiredValidationLayerNames.data() },
#endif
		.enabledExtensionCount{ requiredExtensionCount },
		.ppEnabledExtensionNames{ ppRequiredExtensionNames }
	};

	VkInstance instance;
	if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VkResult::VK_SUCCESS)
		throw std::runtime_error("vkCreateInstance() failed!");

	return instance;
}

VkPhysicalDevice vul::pickSuitedPhysicalDevice(VkInstance const instance)
{
	std::vector<VkPhysicalDevice> const vAvailablePhysicalDevices{ getAvailablePhysicalDevices(instance) };

	const auto suitablePhysicalDeviceIterator
	{
		std::find_if
		(
			vAvailablePhysicalDevices.begin(), vAvailablePhysicalDevices.end(),
			[](VkPhysicalDevice const physicalDevice)
			{
				return isPhysicalDeviceSuitable(physicalDevice);
			}
		)
	};

	if (suitablePhysicalDeviceIterator == vAvailablePhysicalDevices.end())
		throw std::runtime_error("no suitable physical device found!");

	return *suitablePhysicalDeviceIterator;
}

VkDevice vul::createLogicalDevice(VkPhysicalDevice const physicalDevice)
{
	float constexpr queuePriority{ 1.0f };
	VkDeviceQueueCreateInfo const logicalDeviceQueueFamilyCreateInfo
	{
		.sType{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO },
		.queueFamilyIndex{ getAvailableQueueFamiliesIndices(physicalDevice).graphics.value() },
		.queueCount{ 1 },
		.pQueuePriorities{ &queuePriority }
	};

	VkPhysicalDeviceFeatures const enabledPhysicalDeviceFeatures{};

	VkDeviceCreateInfo const logicalDeviceCreateInfo
	{
		.sType{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO },
		.queueCreateInfoCount{ 1 },
		.pQueueCreateInfos{ &logicalDeviceQueueFamilyCreateInfo },
		.pEnabledFeatures{ &enabledPhysicalDeviceFeatures }
	};

	VkDevice logicalDevice;
	if (vkCreateDevice(physicalDevice, &logicalDeviceCreateInfo, nullptr, &logicalDevice) != VkResult::VK_SUCCESS)
		throw std::runtime_error("vkCreateDevice() failed!");

	return logicalDevice;
}

VkQueue vul::getHandleToQueue(VkDevice logicalDevice, std::uint32_t queueFamilyIndex, std::uint32_t queueIndex)
{
	VkQueue queueFamily;
	vkGetDeviceQueue(logicalDevice, queueFamilyIndex, queueIndex, &queueFamily);

	return queueFamily;
}

std::vector<VkExtensionProperties> vul::getAvailableExtensions()
{
	std::uint32_t availableExtensionCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
	std::vector<VkExtensionProperties> vAvaialbleExtensions(availableExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, vAvaialbleExtensions.data());

	return vAvaialbleExtensions;
}

std::vector<VkLayerProperties> vul::getAvailableValidationLayers()
{
	std::uint32_t availableValidationLayersCount;
	vkEnumerateInstanceLayerProperties(&availableValidationLayersCount, nullptr);
	std::vector<VkLayerProperties> vAvaialbleValidationLayers(availableValidationLayersCount);
	vkEnumerateInstanceLayerProperties(&availableValidationLayersCount, vAvaialbleValidationLayers.data());

	return vAvaialbleValidationLayers;
}

std::vector<VkPhysicalDevice> vul::getAvailablePhysicalDevices(VkInstance const instance)
{
	std::uint32_t availablePhysicalDevicesCount;
	vkEnumeratePhysicalDevices(instance, &availablePhysicalDevicesCount, nullptr);
	std::vector<VkPhysicalDevice> vAvailablePhysicalDevices(availablePhysicalDevicesCount);
	vkEnumeratePhysicalDevices(instance, &availablePhysicalDevicesCount, vAvailablePhysicalDevices.data());

	return vAvailablePhysicalDevices;
}

std::vector<VkQueueFamilyProperties> vul::getAvailableQueueFamilies(VkPhysicalDevice const physicalDevice)
{
	std::uint32_t availableQueueFamiliesCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &availableQueueFamiliesCount, nullptr);
	std::vector<VkQueueFamilyProperties> vAvailableQueueFamilies(availableQueueFamiliesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &availableQueueFamiliesCount, vAvailableQueueFamilies.data());

	return vAvailableQueueFamilies;
}

vul::QueueFamilyIndices vul::getAvailableQueueFamiliesIndices(VkPhysicalDevice const physicalDevice)
{
	std::vector<VkQueueFamilyProperties> const vAvailableQueueFamilies{ getAvailableQueueFamilies(physicalDevice) };

	QueueFamilyIndices availableQueueFamilyIndices{};
	int index{};

	for (VkQueueFamilyProperties const& availableQueueFamily : vAvailableQueueFamilies)
	{
		if (availableQueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			availableQueueFamilyIndices.graphics = index;

		++index;

		if (availableQueueFamilyIndices.isComplete())
			break;
	}

	return availableQueueFamilyIndices;
}

bool vul::isExtensionAvailable(std::string_view const extensionName)
{
	std::vector<VkExtensionProperties> const vAvailableExtensions{ getAvailableExtensions() };

	return std::any_of
	(
		vAvailableExtensions.begin(), vAvailableExtensions.end(),
		[extensionName](VkExtensionProperties const& availableExtension)
		{
			return extensionName == availableExtension.extensionName;
		}
	);
}

bool vul::isValidationLayerAvailable(std::string_view const validationLayerName)
{
	std::vector<VkLayerProperties> const vAvaialbleValidationLayers{ getAvailableValidationLayers() };

	return std::any_of
	(
		vAvaialbleValidationLayers.begin(), vAvaialbleValidationLayers.end(),
		[validationLayerName](VkLayerProperties const& availableValidationLayer)
		{
			return validationLayerName == availableValidationLayer.layerName;
		}
	);
}

bool vul::isPhysicalDeviceSuitable(VkPhysicalDevice const physicalDevice)
{
	return getAvailableQueueFamiliesIndices(physicalDevice).isComplete();
}
#pragma endregion HelperFunctions