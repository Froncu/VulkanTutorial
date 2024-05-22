#version 450

layout(binding = 0) uniform UniformBufferObject 
{
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
} uniformBufferObject;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragmentColor;

void main() 
{
    gl_Position =
        uniformBufferObject.projectionMatrix *
        uniformBufferObject.viewMatrix *
        uniformBufferObject.modelMatrix *
        vec4(inPosition, 0.0f, 1.0f);

    fragmentColor = inColor;
}