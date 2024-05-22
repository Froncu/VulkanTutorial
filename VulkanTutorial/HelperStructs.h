#pragma once

#include <Vulkan/vulkan_core.h>
#include <glm/glm.hpp>
#include <optional>
#include <vector>
#include <array>

namespace vul
{
	struct QueueFamilyIndices final
	{
		std::optional<std::uint32_t> graphics{};
		std::optional<std::uint32_t> present{};

		bool isComplete() const;
	};

	struct SwapChainSupportDetails final
	{
		VkSurfaceCapabilitiesKHR capabilities{};
		std::vector<VkSurfaceFormatKHR> vFormats{};
		std::vector<VkPresentModeKHR> vPresentModes{};
	};

	struct Vertex final
	{
		static VkVertexInputBindingDescription getBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();

		glm::vec2 position;
		glm::vec3 color;
	};

	struct UniformBufferObject final
	{
		glm::mat4 modelMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
	};
}