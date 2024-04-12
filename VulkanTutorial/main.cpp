#include "VulkanApplication.h"

#include <vld.h>
#include <stdexcept>
#include <iostream>

int main()
{
	try
	{
		vul::VulkanApplication().run();
	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
	}
}