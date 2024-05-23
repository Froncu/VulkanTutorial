#if not defined fro_WINDOW_H
#define fro_WINDOW_H

#include "Typenames.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <string>

namespace fro
{
	class Window final
	{
	public:
		Window(std::string title, int const width, int const heigth);

		~Window() = default;

		GLFWwindow* getWindow() const;
		int getWidth() const;
		int getHeight() const;

	private:
		static UniquePointer<GLFWwindow> createWindow(std::string_view const title, int const width, int const height);

		Window(Window const&) = delete;
		Window(Window&&) noexcept = delete;

		Window& operator=(Window const&) = delete;
		Window& operator=(Window&&) noexcept = delete;

		UniquePointer<GLFWwindow> const m_pWindow;

		std::string const m_Title;
		int const m_Width;
		int const m_Height;
	};
}

#endif