#include "VulkanApplication.h"

#include "HelperFunctions.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#undef GLFW_INCLUDE_VULKAN
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <stdexcept>
#include <chrono>

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
	m_pDescriptorSetLayout{ createDescriptorSetLayout(m_pLogicalDevice.get()), std::bind(vkDestroyDescriptorSetLayout, m_pLogicalDevice.get(), std::placeholders::_1, nullptr) },
	m_FramesInFlight{ 2 },
	m_pDescriptorPool{ createDescriptorPool(m_FramesInFlight, m_pLogicalDevice.get()), std::bind(vkDestroyDescriptorPool, m_pLogicalDevice.get(), std::placeholders::_1, nullptr) },
	m_pPipelineLayout{ createPipelineLayout(m_pLogicalDevice.get(), m_pDescriptorSetLayout.get()), std::bind(vkDestroyPipelineLayout, m_pLogicalDevice.get(), std::placeholders::_1, nullptr) },
	m_pRenderPass{ createRenderPass(m_SwapChainImageFormat, m_pLogicalDevice.get()), std::bind(vkDestroyRenderPass, m_pLogicalDevice.get(), std::placeholders::_1, nullptr) },
	m_pPipeline{ createPipeline(m_pLogicalDevice.get(), m_SwapChainImageExtent, m_pPipelineLayout.get(), m_pRenderPass.get()), std::bind(vkDestroyPipeline, m_pLogicalDevice.get(), std::placeholders::_1, nullptr) },
	m_vpSwapChainFrameBuffers{ createFramebuffers(m_vpSwapChainImageViews, m_pRenderPass.get(), m_SwapChainImageExtent, m_pLogicalDevice.get()) },
	m_pCommandPool{ createCommandPool(m_PhysicalDevice, m_pWindowSurface.get(), m_pLogicalDevice.get()), std::bind(vkDestroyCommandPool, m_pLogicalDevice.get(), std::placeholders::_1, nullptr) },
	m_vCommandBuffers{ createCommandBuffers(m_pCommandPool.get(), m_pLogicalDevice.get(), m_FramesInFlight) },
	m_vpImageAvailableSemaphores{ createSemaphores(m_pLogicalDevice.get(), m_FramesInFlight) },
	m_vpRenderFinishedSemaphores{ createSemaphores(m_pLogicalDevice.get(), m_FramesInFlight) },
	m_vpInFlightFences{ createFences(m_pLogicalDevice.get(), m_FramesInFlight) },
	m_CurrentFrame{},
	m_FramebufferResized{},
	m_vVertices
	{
		{ { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
		{ { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
		{ { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
		{ { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } }
	},
	m_vIndices{ 0, 1, 2, 2, 3, 0 },
	m_pVertexBuffer{ createVertexBuffer() },
	m_pIndexBuffer{ createIndexBuffer() }
{
	glfwSetWindowUserPointer(m_pWindow.get(), this);
	glfwSetFramebufferSizeCallback(m_pWindow.get(), framebufferResizeCallback);

	createUniformBuffers();
	createDescriptorSets();
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

	updateUniformBuffer();

	recordCommandBuffer(m_vCommandBuffers[m_CurrentFrame], imageIndex, m_pRenderPass.get(), m_vpSwapChainFrameBuffers, m_SwapChainImageExtent, m_pPipeline.get(), m_pVertexBuffer.first.get(), m_pIndexBuffer.first.get(), m_vIndices, m_pPipelineLayout.get(), m_vDescriptorSets, m_CurrentFrame);

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

std::pair<std::unique_ptr<VkBuffer_T, std::function<void(VkBuffer_T*)>>, std::unique_ptr<VkDeviceMemory_T, std::function<void(VkDeviceMemory_T*)>>>
vul::VulkanApplication::createVertexBuffer()
{
	VkDeviceSize const bufferSize{ sizeof(m_vVertices[0]) * m_vVertices.size() };

	auto pStagingBuffer
	{
		createBuffer(m_pLogicalDevice.get(), m_PhysicalDevice,
		bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	};

	void* data;
	vkMapMemory(m_pLogicalDevice.get(), pStagingBuffer.second.get(), 0, bufferSize, 0, &data);
	memcpy(data, m_vVertices.data(), static_cast<std::size_t>(bufferSize));
	vkUnmapMemory(m_pLogicalDevice.get(), pStagingBuffer.second.get());

	auto pVertexBuffer
	{
		createBuffer(m_pLogicalDevice.get(), m_PhysicalDevice,
		bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	};

	copyBuffer(pStagingBuffer.first.get(), pVertexBuffer.first.get(), bufferSize, m_pCommandPool.get(), m_pLogicalDevice.get(), m_GraphicsQueue);

	return pVertexBuffer;
}

std::pair<std::unique_ptr<VkBuffer_T, std::function<void(VkBuffer_T*)>>, std::unique_ptr<VkDeviceMemory_T, std::function<void(VkDeviceMemory_T*)>>>
vul::VulkanApplication::createIndexBuffer()
{
	VkDeviceSize const bufferSize{ sizeof(m_vIndices[0]) * m_vIndices.size() };

	auto pStagingBuffer
	{
		createBuffer(m_pLogicalDevice.get(), m_PhysicalDevice, bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	};

	void* data;
	vkMapMemory(m_pLogicalDevice.get(), pStagingBuffer.second.get(), 0, bufferSize, 0, &data);
	memcpy(data, m_vIndices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(m_pLogicalDevice.get(), pStagingBuffer.second.get());

	auto pIndexBuffer
	{
		createBuffer(m_pLogicalDevice.get(), m_PhysicalDevice, bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	};

	copyBuffer(pStagingBuffer.first.get(), pIndexBuffer.first.get(), bufferSize, m_pCommandPool.get(), m_pLogicalDevice.get(), m_GraphicsQueue);

	return pIndexBuffer;
}

void vul::VulkanApplication::createUniformBuffers()
{
	VkDeviceSize const bufferSize{ sizeof(UniformBufferObject) };

	m_vpUniformBuffers.resize(m_FramesInFlight);
	m_vUniformBuffersMapped.resize(m_FramesInFlight);

	for (std::size_t index{}; index < m_FramesInFlight; ++index)
	{
		m_vpUniformBuffers[index] = createBuffer(m_pLogicalDevice.get(), m_PhysicalDevice,
			bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		vkMapMemory(m_pLogicalDevice.get(), m_vpUniformBuffers[index].second.get(), 0, bufferSize, 0, &m_vUniformBuffersMapped[index]);
	}
}

void vul::VulkanApplication::updateUniformBuffer()
{
	static auto startTime{ std::chrono::high_resolution_clock::now() };

	auto currentTime{ std::chrono::high_resolution_clock::now() };
	float deltaSeconds{ std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count() };

	UniformBufferObject uniformBufferObject
	{
		.modelMatrix{ glm::rotate(glm::mat4(1.0f), deltaSeconds * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) },
		.viewMatrix{ glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)) },
		.projectionMatrix{ glm::perspective(glm::radians(45.0f), m_SwapChainImageExtent.width / static_cast<float>(m_SwapChainImageExtent.height), 0.1f, 10.0f) }
	};

	uniformBufferObject.projectionMatrix[1][1] *= -1;

	memcpy(m_vUniformBuffersMapped[m_CurrentFrame], &uniformBufferObject, sizeof(uniformBufferObject));
}

void vul::VulkanApplication::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> vLayouts(m_FramesInFlight, m_pDescriptorSetLayout.get());
	VkDescriptorSetAllocateInfo const allocationInfo
	{
		.sType{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO  },
		.descriptorPool{ m_pDescriptorPool.get() },
		.descriptorSetCount{ m_FramesInFlight },
		.pSetLayouts{ vLayouts.data() }
	};

	m_vDescriptorSets.resize(m_FramesInFlight);
	if (vkAllocateDescriptorSets(m_pLogicalDevice.get(), &allocationInfo, m_vDescriptorSets.data()) != VK_SUCCESS)
		throw std::runtime_error("vkAllocateDescriptorSets() failed!");

	for (std::size_t index{}; index < m_FramesInFlight; ++index)
	{
		VkDescriptorBufferInfo const bufferInfo
		{
			.buffer{ m_vpUniformBuffers[index].first.get() },
			.offset{ 0 },
			.range{ sizeof(UniformBufferObject) }
		};

		VkWriteDescriptorSet const descriptorWrite
		{
			.sType{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET },
			.dstSet{ m_vDescriptorSets[index] },
			.descriptorCount{ 1 },
			.descriptorType{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
			.pBufferInfo{ &bufferInfo }
		};

		vkUpdateDescriptorSets(m_pLogicalDevice.get(), 1, &descriptorWrite, 0, nullptr);
	}
}

void vul::VulkanApplication::framebufferResizeCallback(GLFWwindow* window, int, int)
{
	VulkanApplication* pApp{ reinterpret_cast<VulkanApplication*>(glfwGetWindowUserPointer(window)) };
	pApp->m_FramebufferResized = true;
}
#pragma endregion PublicMethods