#include "VulkanApplication.h"

#include "HelperFunctions.h"

#include <GLFW/glfw3.h>
#include <xstring>
#include <cstdint>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <format>

#pragma region Constructors/Destructor
vul::VulkanApplication::VulkanApplication() :
	m_pWindow{ createWindow(g_WindowWidth, g_WindowHeight, "Vulkan"), glfwDestroyWindow },
	m_pInstance{ createInstance(), std::bind(vkDestroyInstance, std::placeholders::_1, nullptr) },
	m_PhysicalDevice{ pickSuitedPhysicalDevice(m_pInstance.get()) },
	m_pLogicalDevice{ createLogicalDevice(m_PhysicalDevice), std::bind(vkDestroyDevice, std::placeholders::_1, nullptr) },
	m_GraphicsQueue{ getHandleToQueue(m_pLogicalDevice.get(), getAvailableQueueFamiliesIndices(m_PhysicalDevice).graphics.value(), 0) }
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