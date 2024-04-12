#include "VulkanApplication.h"

#include "HelperFunctions.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#undef GLFW_INCLUDE_VULKAN

#pragma region Constructors/Destructor
vul::VulkanApplication::VulkanApplication() :
	m_pWindow{ createWindow(g_WindowWidth, g_WindowHeight, "Vulkan"), glfwDestroyWindow },
	m_pInstance{ createInstance(), std::bind(vkDestroyInstance, std::placeholders::_1, nullptr) },
	m_pWindowSurface{ createWindowSurface(m_pInstance.get(), m_pWindow.get()), std::bind(vkDestroySurfaceKHR, m_pInstance.get(), std::placeholders::_1, nullptr) },
	m_PhysicalDevice{ pickSuitedPhysicalDevice(m_pInstance.get(), m_pWindowSurface.get(), vPhysicalDeviceExtensionNames) },
	m_pLogicalDevice{ createLogicalDevice(m_PhysicalDevice, m_pWindowSurface.get(), vPhysicalDeviceExtensionNames), std::bind(vkDestroyDevice, std::placeholders::_1, nullptr) },
	m_GraphicsQueue{ getHandleToQueue(m_pLogicalDevice.get(), getAvailableQueueFamiliesIndices(m_PhysicalDevice, m_pWindowSurface.get()).graphics.value(), 0) },
	m_PresentQueue{ getHandleToQueue(m_pLogicalDevice.get(), getAvailableQueueFamiliesIndices(m_PhysicalDevice, m_pWindowSurface.get()).present.value(), 0) },
	m_pSwapChain{ createSwapChain(m_pWindow.get(), m_PhysicalDevice, m_pWindowSurface.get(), m_pLogicalDevice.get(), m_SwapChainImageFormat, m_SwapChainImageExtent), std::bind(vkDestroySwapchainKHR, m_pLogicalDevice.get(), std::placeholders::_1, nullptr) },
	m_vSwapChainImages{ getSwapChainImages(m_pLogicalDevice.get(), m_pSwapChain.get()) },
	m_vpSwapChainImageViews{ createSwapChainImageViews(m_vSwapChainImages, m_SwapChainImageFormat, m_pLogicalDevice.get()) }
{
}

vul::VulkanApplication::~VulkanApplication()
{
	glfwTerminate();
}
#pragma endregion Constructors/Destructor



#pragma region PublicMethods
void vul::VulkanApplication::run()
{
	while (!glfwWindowShouldClose(m_pWindow.get()))
		glfwPollEvents();
}
#pragma endregion PublicMethods