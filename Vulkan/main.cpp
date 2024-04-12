#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLM/vec4.hpp>
#include <GLM/mat4x4.hpp>

#include <vld.h>
#include <iostream>
#include <format>

int main() 
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* const pWindow{ glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr) };

    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::cout << std::format("{} extensions supported\n", extensionCount);

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test{ matrix * vec };

    while (!glfwWindowShouldClose(pWindow))
        glfwPollEvents();

    glfwDestroyWindow(pWindow);

    glfwTerminate();

    return 0;
}