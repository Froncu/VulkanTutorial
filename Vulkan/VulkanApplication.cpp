#include "VulkanApplication.h"

#include <GLFW/glfw3.h>
#include <cstdint>
#include <stdexcept>

#pragma region Constructors/Destructor
vul::VulkanApplication::VulkanApplication() :
	m_pWindow{ createWindow() },
	m_Instance{ createInstance() }
{
};

vul::VulkanApplication::~VulkanApplication()
{
	vkDestroyInstance(m_Instance, nullptr);

	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
};
#pragma endregion Constructors/Destructor



#pragma region PublicMethods
void vul::VulkanApplication::run()
{
	while (!glfwWindowShouldClose(m_pWindow))
		glfwPollEvents();
}
#pragma endregion PublicMethods



#pragma region PrivateMethods
GLFWwindow* vul::VulkanApplication::createWindow() const
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	return glfwCreateWindow(g_WindowWidth, g_WindowHeight, "Vulkan", nullptr, nullptr);
}

VkInstance vul::VulkanApplication::createInstance() const
{
	std::uint32_t GLFWExtensionCount;
	char const* const* const ppRequiredGLFWExtensions{ glfwGetRequiredInstanceExtensions(&GLFWExtensionCount) };

	VkInstanceCreateInfo const instanceCreateInfo
	{
		.sType{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO },
		.enabledExtensionCount{ GLFWExtensionCount },
		.ppEnabledExtensionNames{ ppRequiredGLFWExtensions }
	};

	VkInstance instance;
	if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VkResult::VK_SUCCESS)
		throw std::runtime_error("vkCreateInstance() failed!");

	return instance;
}
#pragma endregion PrivateMethods