#pragma once

#include <Vulkan/vulkan_core.h>
#include <optional>
#include <vector>

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
}