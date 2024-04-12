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

[[nodiscard("handle to created instance ignored!")]] VkInstance createInstance()
{
	std::uint32_t requiredExtensionCount;
	char const* const* const ppRequiredExtensionNames{ glfwGetRequiredInstanceExtensions(&requiredExtensionCount) };

	const std::vector<std::string_view> vRequiredExtensionNames{ ppRequiredExtensionNames, ppRequiredExtensionNames + requiredExtensionCount };
	for (std::string_view requiredExtensionName : vRequiredExtensionNames)
		if (!isExtensionAvailable(requiredExtensionName))
			throw std::runtime_error(std::format("extension {} is not available!", requiredExtensionName));

	VkInstanceCreateInfo const instanceCreateInfo
	{
		.sType{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO },
		.enabledExtensionCount{ requiredExtensionCount },
		.ppEnabledExtensionNames{ ppRequiredExtensionNames }
	};

	VkInstance instance;
	if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VkResult::VK_SUCCESS)
		throw std::runtime_error("vkCreateInstance() failed!");

	return instance;
}
#pragma endregion HelperFunctions



#pragma region Constructors/Destructor
vul::VulkanApplication::VulkanApplication() :
	m_pWindow{ createWindow(g_WindowWidth, g_WindowHeight, "Vulkan"), glfwDestroyWindow },
	m_pInstance{ createInstance(), std::bind(vkDestroyInstance, std::placeholders::_1, nullptr) }
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