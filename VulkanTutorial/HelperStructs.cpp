#include "HelperStructs.h"

bool vul::QueueFamilyIndices::isComplete() const
{
	return
		graphics.has_value() and
		present.has_value();
}

VkVertexInputBindingDescription vul::Vertex::getBindingDescription()
{
	return VkVertexInputBindingDescription
	{
		.stride{ sizeof(Vertex) },
		.inputRate{ VK_VERTEX_INPUT_RATE_VERTEX }
	};
}

std::array<VkVertexInputAttributeDescription, 3> vul::Vertex::getAttributeDescriptions()
{
	return std::array<VkVertexInputAttributeDescription, 3>
	{
		VkVertexInputAttributeDescription
		{
			.location{ 0 },
			.binding{ 0 },
			.format{ VK_FORMAT_R32G32_SFLOAT },
			.offset{ offsetof(Vertex, position) }
		},

		VkVertexInputAttributeDescription
		{
			.location{ 1 },
			.binding{ 0 },
			.format{ VK_FORMAT_R32G32B32_SFLOAT },
			.offset{ offsetof(Vertex, color) }
		},

		VkVertexInputAttributeDescription
		{
			.location{ 2 },
			.binding{ 0 },
			.format{ VK_FORMAT_R32G32_SFLOAT },
			.offset{ offsetof(Vertex, texCoord) }
		}
	};
}
