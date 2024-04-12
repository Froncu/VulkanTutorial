#include "VulkanApplication.h"

#include <GLFW/glfw3.h>
#include <xstring>
#include <cstdint>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <format>

#pragma region HelperFunctions
[[nodiscard("handle to created window ignored!")]] GLFWwindow* createWindow(int width, int height, std::string_view title)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	return glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
}

std::vector<VkExtensionProperties> getAvailableExtensions()
{
	std::uint32_t availableExtensionCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
	std::vector<VkExtensionProperties> vAvaialbleExtensions(availableExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, vAvaialbleExtensions.data());

	return vAvaialbleExtensions;
}

bool isExtensionAvailable(std::string_view extensionName)
{
	const std::vector<VkExtensionProperties> vAvailableExtensions{ getAvailableExtensions() };

	return std::any_of
	(
		vAvailableExtensions.begin(), vAvailableExtensions.end(),
		[extensionName](const VkExtensionProperties& availableExtension)
		{
			return extensionName == availableExtension.extensionName;
		}
	);
};

std::vector<VkLayerProperties> getAvailableValidationLayers()
{
	std::uint32_t availableValidationLayersCount;
	vkEnumerateInstanceLayerProperties(&availableValidationLayersCount, nullptr);
	std::vector<VkLayerProperties> vAvaialbleValidationLayers(availableValidationLayersCount);
	vkEnumerateInstanceLayerProperties(&availableValidationLayersCount, vAvaialbleValidationLayers.data());

	return vAvaialbleValidationLayers;
}

bool isValidationLayerAvailable(std::string_view validationLayerName)
{
	const std::vector<VkLayerProperties> vAvaialbleValidationLayers{ getAvailableValidationLayers() };

	return std::any_of
	(
		vAvaialbleValidationLayers.begin(), vAvaialbleValidationLayers.end(),
		[validationLayerName](const VkLayerProperties& availableValidationLayer)
		{
			return validationLayerName == availableValidationLayer.layerName;
		}
	);
};

[[nodiscard("handle to created instance ignored!")]] VkInstance createInstance()
{
#ifndef NDEBUG
	const std::vector vRequiredValidationLayerNames{ "VK_LAYER_KHRONOS_validation" };

	for (std::string_view requiredValidationLayerName : vRequiredValidationLayerNames)
		if (!isValidationLayerAvailable(requiredValidationLayerName))
			throw std::runtime_error(std::format("validation layer {} is not available!", requiredValidationLayerName));
#endif

	std::uint32_t requiredExtensionCount;
	char const* const* const ppRequiredExtensionNames{ glfwGetRequiredInstanceExtensions(&requiredExtensionCount) };

	const std::vector<std::string_view> vRequiredExtensionNames{ ppRequiredExtensionNames, ppRequiredExtensionNames + requiredExtensionCount };
	for (std::string_view requiredExtensionName : vRequiredExtensionNames)
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

std::vector<VkPhysicalDevice> getAvailablePhysicalDevices(VkInstance instance)
{
	std::uint32_t availablePhysicalDevicesCount;
	vkEnumeratePhysicalDevices(instance, &availablePhysicalDevicesCount, nullptr);
	std::vector<VkPhysicalDevice> vAvailablePhysicalDevices(availablePhysicalDevicesCount);
	vkEnumeratePhysicalDevices(instance, &availablePhysicalDevicesCount, vAvailablePhysicalDevices.data());

	return vAvailablePhysicalDevices;
}

std::vector<VkQueueFamilyProperties> getAvailableQueueFamilies(VkPhysicalDevice physicalDevice)
{
	std::uint32_t availableQueueFamiliesCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &availableQueueFamiliesCount, nullptr);
	std::vector<VkQueueFamilyProperties> vAvailableQueueFamilies(availableQueueFamiliesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &availableQueueFamiliesCount, vAvailableQueueFamilies.data());

	return vAvailableQueueFamilies;
}

struct QueueFamilyIndices final
{
	std::optional<std::uint32_t> graphics{};

	bool isComplete() const
	{
		return graphics.has_value();
	}
};

QueueFamilyIndices getAvailableQueueFamiliesIndices(VkPhysicalDevice physicalDevice)
{
	const std::vector<VkQueueFamilyProperties> vAvailableQueueFamilies{ getAvailableQueueFamilies(physicalDevice) };

	QueueFamilyIndices availableQueueFamilyIndices{};
	int index{};

	for (const VkQueueFamilyProperties& availableQueueFamily : vAvailableQueueFamilies)
	{
		if (availableQueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			availableQueueFamilyIndices.graphics = index;

		++index;

		if (availableQueueFamilyIndices.isComplete())
			break;
	}

	return availableQueueFamilyIndices;
}

bool isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice)
{
	return getAvailableQueueFamiliesIndices(physicalDevice).isComplete();
}

VkPhysicalDevice pickSuitedPhysicalDevice(VkInstance instance)
{
	const std::vector<VkPhysicalDevice> vAvailablePhysicalDevices{ getAvailablePhysicalDevices(instance) };

	const auto suitablePhysicalDeviceIterator
	{
		std::find_if
		(
			vAvailablePhysicalDevices.begin(), vAvailablePhysicalDevices.end(),
			[](VkPhysicalDevice physicalDevice)
			{
				return isPhysicalDeviceSuitable(physicalDevice);
			}
		)
	};

	if (suitablePhysicalDeviceIterator == vAvailablePhysicalDevices.end())
		throw std::runtime_error("no suitable physical device found!");

	return *suitablePhysicalDeviceIterator;
}
#pragma endregion HelperFunctions



#pragma region Constructors/Destructor
vul::VulkanApplication::VulkanApplication() :
	m_pWindow{ createWindow(g_WindowWidth, g_WindowHeight, "Vulkan"), glfwDestroyWindow },
	m_pInstance{ createInstance(), std::bind(vkDestroyInstance, std::placeholders::_1, nullptr) },
	m_PhysicalDevice{ pickSuitedPhysicalDevice(m_pInstance.get()) }
{
};

vul::VulkanApplication::~VulkanApplication()
{
	glfwTerminate();
};
#pragma endregion Constructors/Destructor



#pragma region PublicMethods
void vul::VulkanApplication::run()
{
	while (!glfwWindowShouldClose(m_pWindow.get()))
		glfwPollEvents();
}
#pragma endregion PublicMethods