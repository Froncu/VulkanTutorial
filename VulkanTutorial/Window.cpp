#include "Window.h"

#pragma region Constructors/Destructor
fro::Window::Window(std::string title, int const width, int const heigth)
	: m_pWindow{ createWindow(title, width, heigth) }
	, m_Title{ title }
	, m_Width{ width }
	, m_Height{ heigth }
{
}
#pragma endregion Constructors/Destructor



#pragma region PublicMethods
GLFWwindow* fro::Window::getWindow() const
{
	return m_pWindow.get();
}

int fro::Window::getWidth() const
{
	return m_Width;
}

int fro::Window::getHeight() const
{
	return m_Height;
}
#pragma endregion PublicMethods



#pragma region PrivateMethods
fro::UniquePointer<GLFWwindow> fro::Window::createWindow(std::string_view const title, int const width, int const height)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	return
	{
		glfwCreateWindow(width, height, title.data(), nullptr, nullptr),
		glfwDestroyWindow
	};
}
#pragma endregion PrivateMethods