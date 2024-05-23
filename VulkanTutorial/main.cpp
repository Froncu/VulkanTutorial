#include "VulkanApplication.h"

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>

int main()
{
	glfwInit();

	try
	{
		fro::VulkanApplication().run();
	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
	}
}