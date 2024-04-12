#include "HelperFunctions.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#undef GLFW_INCLUDE_VULKAN
#include <algorithm>
#include <stdexcept>
#include <format>
#include <set>

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
		if (!isInstanceExtensionAvailable(requiredExtensionName))
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

VkSurfaceKHR vul::createWindowSurface(VkInstance const instance, GLFWwindow* const pWindow)
{
	VkSurfaceKHR windowSurface;
	if (glfwCreateWindowSurface(instance, pWindow, nullptr, &windowSurface) != VkResult::VK_SUCCESS)
		throw std::runtime_error("glfwCreateWindowSurface() failed!");

	return windowSurface;
}

VkPhysicalDevice vul::pickSuitedPhysicalDevice(VkInstance const instance, VkSurfaceKHR const windowSurface, std::vector<std::string_view> const& vPhyicalDeviceExtensionNames)
{
	std::vector<VkPhysicalDevice> const vAvailablePhysicalDevices{ getAvailablePhysicalDevices(instance) };

	const auto suitablePhysicalDeviceIterator
	{
		std::find_if
		(
			vAvailablePhysicalDevices.begin(), vAvailablePhysicalDevices.end(),
			[windowSurface, &vPhyicalDeviceExtensionNames](VkPhysicalDevice const physicalDevice)
			{
				return isPhysicalDeviceSuitable(physicalDevice, windowSurface, vPhyicalDeviceExtensionNames);
			}
		)
	};

	if (suitablePhysicalDeviceIterator == vAvailablePhysicalDevices.end())
		throw std::runtime_error("no suitable physical device found!");

	return *suitablePhysicalDeviceIterator;
}

VkDevice vul::createLogicalDevice(VkPhysicalDevice const physicalDevice, VkSurfaceKHR const windowSurface, std::vector<std::string_view> const& vPhyicalDeviceExtensionNames)
{
	QueueFamilyIndices const availableQueueFamilyIndices{ getAvailableQueueFamiliesIndices(physicalDevice, windowSurface) };

	std::set<std::uint32_t> sAvailableUniqueQueueFamilyIndices{ availableQueueFamilyIndices.graphics.value(), availableQueueFamilyIndices.present.value() };
	std::vector<VkDeviceQueueCreateInfo> vLogicalDeviceQueueFamilyCreateInfos{};
	for (std::uint32_t availableUniqueQueueFamilyIndex : sAvailableUniqueQueueFamilyIndices)
	{
		float constexpr queuePriority{ 1.0f };
		vLogicalDeviceQueueFamilyCreateInfos.emplace_back
		(
			VkDeviceQueueCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = availableUniqueQueueFamilyIndex,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority
			}
		);
	}

	VkPhysicalDeviceFeatures const enabledPhysicalDeviceFeatures{};

	std::vector<char const*> vpPhyicalDeviceExtensionNames{};
	for (std::string_view physicalDeviceExtensionName : vPhyicalDeviceExtensionNames)
		vpPhyicalDeviceExtensionNames.push_back(physicalDeviceExtensionName.data());

	VkDeviceCreateInfo const logicalDeviceCreateInfo
	{
		.sType{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO },
		.queueCreateInfoCount{ static_cast<std::uint32_t>(vLogicalDeviceQueueFamilyCreateInfos.size()) },
		.pQueueCreateInfos{ vLogicalDeviceQueueFamilyCreateInfos.data() },
		.enabledExtensionCount{ static_cast<std::uint32_t>(vPhyicalDeviceExtensionNames.size()) },
		.ppEnabledExtensionNames{ vpPhyicalDeviceExtensionNames.data() },
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

VkSwapchainKHR vul::createSwapChain(GLFWwindow* const pWindow, VkPhysicalDevice const physicalDevice, VkSurfaceKHR const windowSurface, VkDevice const logicalDevice, VkFormat& swapChainImageFormat, VkExtent2D& swapChainImageExtent)
{
	SwapChainSupportDetails const swapChainSupportDetails{ getSwapChainSupportDetails(physicalDevice, windowSurface) };

	VkSurfaceCapabilitiesKHR const& swapChainCapabilties{ swapChainSupportDetails.capabilities };

	VkExtent2D localSwapChainImageExtent;
	if (swapChainCapabilties.currentExtent.width != std::numeric_limits<std::uint32_t>::max())
		localSwapChainImageExtent = swapChainCapabilties.currentExtent;
	else
	{
		int width;
		int height;
		glfwGetFramebufferSize(pWindow, &width, &height);

		localSwapChainImageExtent.width = std::clamp(static_cast<std::uint32_t>(width), swapChainCapabilties.minImageExtent.width, swapChainCapabilties.maxImageExtent.width);
		localSwapChainImageExtent.height = std::clamp(static_cast<std::uint32_t>(height), swapChainCapabilties.minImageExtent.height, swapChainCapabilties.maxImageExtent.height);
	}

	std::uint32_t imageCount{ swapChainCapabilties.minImageCount + 1 };
	if (swapChainCapabilties.maxImageCount > 0 && imageCount > swapChainCapabilties.maxImageCount)
		imageCount = swapChainCapabilties.maxImageCount;

	auto const& vAvailableSwapChainSurfaceFormats{ swapChainSupportDetails.vFormats };
	auto swapChainSurfaceFormatIterator
	{
		std::find_if
		(
			vAvailableSwapChainSurfaceFormats.begin(), vAvailableSwapChainSurfaceFormats.end(),
			[](VkSurfaceFormatKHR availableSwapChainFormat)
			{
				return
					availableSwapChainFormat.format == VK_FORMAT_B8G8R8A8_SRGB and
					availableSwapChainFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			}
		)
	};
	if (swapChainSurfaceFormatIterator == vAvailableSwapChainSurfaceFormats.end())
		swapChainSurfaceFormatIterator = vAvailableSwapChainSurfaceFormats.begin();

	auto const& vAvailableSwapChainPresentModes{ swapChainSupportDetails.vPresentModes };
	auto swapChainPresentModeIterator
	{
		std::find_if
		(
			vAvailableSwapChainPresentModes.begin(), vAvailableSwapChainPresentModes.end(),
			[](VkPresentModeKHR availablePresentMode)
			{
				return availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR;
			}
		)
	};
	if (swapChainPresentModeIterator == vAvailableSwapChainPresentModes.end())
	{
		swapChainPresentModeIterator = std::find_if
		(
			vAvailableSwapChainPresentModes.begin(), vAvailableSwapChainPresentModes.end(),
			[](VkPresentModeKHR availablePresentMode)
			{
				return availablePresentMode == VK_PRESENT_MODE_FIFO_KHR;
			}
		);

		if (swapChainPresentModeIterator == vAvailableSwapChainPresentModes.end())
			throw std::runtime_error("no suitable present mode available!");
	}

	QueueFamilyIndices availableQueueFamilyIndices{ getAvailableQueueFamiliesIndices(physicalDevice, windowSurface) };
	uint32_t const aQueueFamilyIndices[]{ availableQueueFamilyIndices.graphics.value(), availableQueueFamilyIndices.present.value() };

	VkSharingMode imageSharingMode;
	uint32_t queueFamilyIndexCount;
	uint32_t const* pQueueFamilyIndices;
	if (availableQueueFamilyIndices.graphics != availableQueueFamilyIndices.present) 
	{
		imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		queueFamilyIndexCount = 2;
		pQueueFamilyIndices = aQueueFamilyIndices;
	}
	else 
	{
		imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		queueFamilyIndexCount = 0;
		pQueueFamilyIndices = nullptr;
	}

	VkSwapchainCreateInfoKHR const swapChainCreateInfo
	{
		.sType{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR },
		.surface{ windowSurface },
		.minImageCount{ imageCount },
		.imageFormat{ swapChainSurfaceFormatIterator->format },
		.imageColorSpace{ swapChainSurfaceFormatIterator->colorSpace },
		.imageExtent{ localSwapChainImageExtent },
		.imageArrayLayers{ 1 },
		.imageUsage{ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT },
		.imageSharingMode{ imageSharingMode },
		.queueFamilyIndexCount{ queueFamilyIndexCount },
		.pQueueFamilyIndices{ pQueueFamilyIndices },
		.preTransform{ swapChainCapabilties.currentTransform },
		.compositeAlpha{ VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR },
		.presentMode{ *swapChainPresentModeIterator },
		.clipped{ VK_TRUE },
		.oldSwapchain{ VK_NULL_HANDLE }
	};

	VkSwapchainKHR swapChain;
	if (vkCreateSwapchainKHR(logicalDevice, &swapChainCreateInfo, nullptr, &swapChain) != VK_SUCCESS)
		throw std::runtime_error("vkCreateSwapchainKHR() failed!");

	swapChainImageFormat = swapChainCreateInfo.imageFormat;
	swapChainImageExtent = swapChainCreateInfo.imageExtent;

	return swapChain;
}

std::vector<VkExtensionProperties> vul::getAvailableInstanceExtensions()
{
	std::uint32_t availableInstanceExtensionCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &availableInstanceExtensionCount, nullptr);
	std::vector<VkExtensionProperties> vAvaialbleInstanceExtensions(availableInstanceExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &availableInstanceExtensionCount, vAvaialbleInstanceExtensions.data());

	return vAvaialbleInstanceExtensions;
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

std::vector<VkExtensionProperties> vul::getAvailablePhysicalDeviceExtensions(VkPhysicalDevice const physicalDevice)
{
	std::uint32_t availablePhysicalDeviceExtensionCount;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availablePhysicalDeviceExtensionCount, nullptr);
	std::vector<VkExtensionProperties> vAvaialblePhysicalDeviceExtensions(availablePhysicalDeviceExtensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availablePhysicalDeviceExtensionCount, vAvaialblePhysicalDeviceExtensions.data());

	return vAvaialblePhysicalDeviceExtensions;
}

std::vector<VkQueueFamilyProperties> vul::getAvailableQueueFamilies(VkPhysicalDevice const physicalDevice)
{
	std::uint32_t availableQueueFamiliesCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &availableQueueFamiliesCount, nullptr);
	std::vector<VkQueueFamilyProperties> vAvailableQueueFamilies(availableQueueFamiliesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &availableQueueFamiliesCount, vAvailableQueueFamilies.data());

	return vAvailableQueueFamilies;
}

std::vector<VkSurfaceFormatKHR> vul::getAvailablePhysicalDeviceWindowSurfaceFormats(VkPhysicalDevice const physicalDevice, VkSurfaceKHR const windowSurface)
{
	std::uint32_t availableSurfaceFormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, windowSurface, &availableSurfaceFormatCount, nullptr);
	std::vector<VkSurfaceFormatKHR> vAvailableSurfaceFormats(availableSurfaceFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, windowSurface, &availableSurfaceFormatCount, vAvailableSurfaceFormats.data());

	return vAvailableSurfaceFormats;
}

std::vector<VkPresentModeKHR> vul::getAvailablePhysicalDeviceWindowSurfacePresentModes(VkPhysicalDevice const physicalDevice, VkSurfaceKHR const windowSurface)
{
	std::uint32_t availableSurfacePresentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, windowSurface, &availableSurfacePresentModeCount, nullptr);
	std::vector<VkPresentModeKHR> vAvailableSurfacePresentModes(availableSurfacePresentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, windowSurface, &availableSurfacePresentModeCount, vAvailableSurfacePresentModes.data());

	return vAvailableSurfacePresentModes;
}

std::vector<VkImage> vul::getSwapChainImages(VkDevice const logicalDevice, VkSwapchainKHR const swapChain)
{
	std::uint32_t swapChainImageCount;
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &swapChainImageCount, nullptr);
	std::vector<VkImage> vSwapChainImages(swapChainImageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &swapChainImageCount, vSwapChainImages.data());

	return vSwapChainImages;
}

vul::QueueFamilyIndices vul::getAvailableQueueFamiliesIndices(VkPhysicalDevice const physicalDevice, VkSurfaceKHR const windowSurface)
{
	std::vector<VkQueueFamilyProperties> const vAvailableQueueFamilies{ getAvailableQueueFamilies(physicalDevice) };

	QueueFamilyIndices availableQueueFamilyIndices{};
	int index{};

	for (VkQueueFamilyProperties const& availableQueueFamily : vAvailableQueueFamilies)
	{
		if (availableQueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			availableQueueFamilyIndices.graphics = index;

		VkBool32 isPresentingToWindowSurfaceSupported;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, windowSurface, &isPresentingToWindowSurfaceSupported);
		if (isPresentingToWindowSurfaceSupported)
			availableQueueFamilyIndices.present = index;

		++index;

		if (availableQueueFamilyIndices.isComplete())
			break;
	}

	return availableQueueFamilyIndices;
}

vul::SwapChainSupportDetails vul::getSwapChainSupportDetails(VkPhysicalDevice const physicalDevice, VkSurfaceKHR const windowSurface)
{
	VkSurfaceCapabilitiesKHR windowSurfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, windowSurface, &windowSurfaceCapabilities);

	SwapChainSupportDetails const swapChainSupportDetails
	{
		.capabilities{ windowSurfaceCapabilities },
		.vFormats{ getAvailablePhysicalDeviceWindowSurfaceFormats(physicalDevice, windowSurface) },
		.vPresentModes{ getAvailablePhysicalDeviceWindowSurfacePresentModes(physicalDevice, windowSurface) }
	};

	return swapChainSupportDetails;
}

bool vul::isInstanceExtensionAvailable(std::string_view const instanceExtensionName)
{
	std::vector<VkExtensionProperties> const vAvailableInstanceExtensions{ getAvailableInstanceExtensions() };

	return std::any_of
	(
		vAvailableInstanceExtensions.begin(), vAvailableInstanceExtensions.end(),
		[instanceExtensionName](VkExtensionProperties const& availableInstanceExtension)
		{
			return instanceExtensionName == availableInstanceExtension.extensionName;
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

bool vul::isPhysicalDeviceExtensionAvailable(std::string_view const physicalDeviceExtensionName, VkPhysicalDevice physicalDevice)
{
	std::vector<VkExtensionProperties> const vAvailablePhysicalDeviceExtensions{ getAvailablePhysicalDeviceExtensions(physicalDevice) };

	return std::any_of
	(
		vAvailablePhysicalDeviceExtensions.begin(), vAvailablePhysicalDeviceExtensions.end(),
		[physicalDeviceExtensionName](VkExtensionProperties const& availablePhysicalDeviceExtension)
		{
			return physicalDeviceExtensionName == availablePhysicalDeviceExtension.extensionName;
		}
	);
}

bool vul::isPhysicalDeviceSuitable(VkPhysicalDevice const physicalDevice, VkSurfaceKHR const windowSurface, std::vector<std::string_view> const& vPhyicalDeviceExtensionNames)
{
	SwapChainSupportDetails const& swapChainSupportDetails{ getSwapChainSupportDetails(physicalDevice, windowSurface) };

	return
		getAvailableQueueFamiliesIndices(physicalDevice, windowSurface).isComplete() and
		std::all_of
		(
			vPhyicalDeviceExtensionNames.begin(), vPhyicalDeviceExtensionNames.end(),
			[physicalDevice](std::string_view const physicalDeviceExtensionName)
			{
				return isPhysicalDeviceExtensionAvailable(physicalDeviceExtensionName, physicalDevice);
			}
		) and
		not swapChainSupportDetails.vFormats.empty() and
				not swapChainSupportDetails.vPresentModes.empty();
}
#pragma endregion HelperFunctions