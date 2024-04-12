#pragma once

struct GLFWwindow;

namespace vul
{
	constexpr int g_WindowWidth{ 800 };
	constexpr int g_WindowHeight{ 600 };

	class VulkanApplication final
	{
	public:
		VulkanApplication();
		VulkanApplication(const VulkanApplication&) = delete;
		VulkanApplication(VulkanApplication&&) noexcept = delete;

		~VulkanApplication();

		VulkanApplication& operator=(const VulkanApplication&) = delete;
		VulkanApplication& operator=(VulkanApplication&&) noexcept = delete;

		void run();

	private:
		GLFWwindow* createWindow() const;

		GLFWwindow* const m_pWindow;
	};
}