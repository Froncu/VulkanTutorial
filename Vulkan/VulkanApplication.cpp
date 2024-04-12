#include "VulkanApplication.h"

#include <GLFW/glfw3.h>

#pragma region Constructors/Destructor
vul::VulkanApplication::VulkanApplication() :
	m_pWindow{ createWindow() }
{
};

vul::VulkanApplication::~VulkanApplication()
{
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
#pragma endregion PrivateMethods