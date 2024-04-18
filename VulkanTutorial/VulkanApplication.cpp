#include "VulkanApplication.h"

#include "HelperFunctions.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#undef GLFW_INCLUDE_VULKAN
#include <stdexcept>

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
	m_vpSwapChainImageViews{ createSwapChainImageViews(m_vSwapChainImages, m_SwapChainImageFormat, m_pLogicalDevice.get()) },
	m_pPipelineLayout{ createPipelineLayout(m_pLogicalDevice.get()), std::bind(vkDestroyPipelineLayout, m_pLogicalDevice.get(), std::placeholders::_1, nullptr) },
	m_pRenderPass{ createRenderPass(m_SwapChainImageFormat, m_pLogicalDevice.get()), std::bind(vkDestroyRenderPass, m_pLogicalDevice.get(), std::placeholders::_1, nullptr) },
	m_pPipeline{ createPipeline(m_pLogicalDevice.get(), m_SwapChainImageExtent, m_pPipelineLayout.get(), m_pRenderPass.get()), std::bind(vkDestroyPipeline, m_pLogicalDevice.get(), std::placeholders::_1, nullptr) },
	m_vpSwapChainFrameBuffers{ createFramebuffers(m_vpSwapChainImageViews, m_pRenderPass.get(), m_SwapChainImageExtent, m_pLogicalDevice.get()) },
	m_pCommandPool{ createCommandPool(m_PhysicalDevice, m_pWindowSurface.get(), m_pLogicalDevice.get()), std::bind(vkDestroyCommandPool, m_pLogicalDevice.get(), std::placeholders::_1, nullptr) },
	m_FramesInFlight{ 2 },
	m_vCommandBuffers{ createCommandBuffers(m_pCommandPool.get(), m_pLogicalDevice.get(), m_FramesInFlight) },
	m_vpImageAvailableSemaphores{ createSemaphores(m_pLogicalDevice.get(), m_FramesInFlight) },
	m_vpRenderFinishedSemaphores{ createSemaphores(m_pLogicalDevice.get(), m_FramesInFlight) },
	m_vpInFlightFences{ createFences(m_pLogicalDevice.get(), m_FramesInFlight) },
	m_CurrentFrame{},
	m_FramebufferResized{}
{
	glfwSetWindowUserPointer(m_pWindow.get(), this);
	glfwSetFramebufferSizeCallback(m_pWindow.get(), framebufferResizeCallback);
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
	{
		glfwPollEvents();
		render();
	}

	vkDeviceWaitIdle(m_pLogicalDevice.get());
}

void vul::VulkanApplication::render()
{
	VkFence aFences[]{ m_vpInFlightFences[m_CurrentFrame].get()};
	vkWaitForFences(m_pLogicalDevice.get(), 1, aFences, VK_TRUE, UINT64_MAX);

	std::uint32_t imageIndex;
	VkResult result{ vkAcquireNextImageKHR(m_pLogicalDevice.get(), m_pSwapChain.get(), UINT64_MAX, m_vpImageAvailableSemaphores[m_CurrentFrame].get(), VK_NULL_HANDLE, &imageIndex) };
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		m_FramebufferResized = true;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
		throw std::runtime_error("vkAcquireNextImageKHR() failed!");

	vkResetFences(m_pLogicalDevice.get(), 1, aFences);

	vkResetCommandBuffer(m_vCommandBuffers[m_CurrentFrame], 0);

	recordCommandBuffer(m_vCommandBuffers[m_CurrentFrame], imageIndex, m_pRenderPass.get(), m_vpSwapChainFrameBuffers, m_SwapChainImageExtent, m_pPipeline.get());

	VkSemaphore const aWaitSemaphores[]{ m_vpImageAvailableSemaphores[m_CurrentFrame].get() };
	VkSemaphore const aSignalSemaphores[]{ m_vpRenderFinishedSemaphores[m_CurrentFrame].get() };
	VkPipelineStageFlags const aWaitStages[]{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSubmitInfo const submitInfo
	{
		.sType{ VK_STRUCTURE_TYPE_SUBMIT_INFO },
		.waitSemaphoreCount{ 1 },
		.pWaitSemaphores{ aWaitSemaphores },
		.pWaitDstStageMask{ aWaitStages },
		.commandBufferCount{ 1 },
		.pCommandBuffers{ &m_vCommandBuffers[m_CurrentFrame] },
		.signalSemaphoreCount{ 1 },
		.pSignalSemaphores{ aSignalSemaphores }
	};

	if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_vpInFlightFences[m_CurrentFrame].get()) != VK_SUCCESS)
		throw std::runtime_error("vkQueueSubmit() failed!");

	VkSwapchainKHR aSwapChains[]{ m_pSwapChain.get() };
	VkPresentInfoKHR const presentInfo
	{
		.sType{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR },
		.waitSemaphoreCount{ 1 },
		.pWaitSemaphores{ aSignalSemaphores },
		.swapchainCount{ 1 },
		.pSwapchains{ aSwapChains },
		.pImageIndices{ &imageIndex },
	};

	result = vkQueuePresentKHR(m_PresentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_FramebufferResized) 
	{
		m_FramebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
		throw std::runtime_error("failed to present swap chain image!");

	m_CurrentFrame = (m_CurrentFrame + 1) % m_FramesInFlight;
}

void vul::VulkanApplication::recreateSwapChain()
{
	int width{};
	int height{};

	glfwGetFramebufferSize(m_pWindow.get(), &width, &height);
	while (width == 0 || height == 0) 
	{
		glfwGetFramebufferSize(m_pWindow.get(), &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_pLogicalDevice.get());

	m_vpSwapChainFrameBuffers.clear();
	m_vpSwapChainImageViews.clear();
	m_vSwapChainImages.clear();
	m_pSwapChain.reset();

	m_pSwapChain =
	{
		createSwapChain(m_pWindow.get(), m_PhysicalDevice, m_pWindowSurface.get(), m_pLogicalDevice.get(), m_SwapChainImageFormat, m_SwapChainImageExtent), 
		std::bind(vkDestroySwapchainKHR, m_pLogicalDevice.get(), std::placeholders::_1, nullptr)
	};
	m_vSwapChainImages = getSwapChainImages(m_pLogicalDevice.get(), m_pSwapChain.get());
	m_vpSwapChainImageViews = createSwapChainImageViews(m_vSwapChainImages, m_SwapChainImageFormat, m_pLogicalDevice.get());
	m_vpSwapChainFrameBuffers = createFramebuffers(m_vpSwapChainImageViews, m_pRenderPass.get(), m_SwapChainImageExtent, m_pLogicalDevice.get());
}

void vul::VulkanApplication::framebufferResizeCallback(GLFWwindow* window, int, int)
{
	VulkanApplication* pApp{ reinterpret_cast<VulkanApplication*>(glfwGetWindowUserPointer(window)) };
	pApp->m_FramebufferResized = true;
}
#pragma endregion PublicMethods