#include "VulkanApplication.h"

#include <GLFW/glfw3.h>
#include <xstring>
#include <cstdint>
#include <stdexcept>

#pragma region HelperFunctions
[[nodiscard("handle to created window ignored!")]] GLFWwindow* createWindow(int width, int height, std::string_view title)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	return glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
}

[[nodiscard("handle to created instance ignored!")]] VkInstance createInstance()
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
#pragma endregion HelperFunctions



#pragma region Constructors/Destructor
vul::VulkanApplication::VulkanApplication() :
	m_pWindow{ createWindow(g_WindowWidth, g_WindowHeight, "Vulkan") },
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