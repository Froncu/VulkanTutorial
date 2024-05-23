#version 450

layout(location = 0) in vec3 fragmentColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outputColor;

layout(binding = 1) uniform sampler2D texSampler;

void main()
{
    outputColor = texture(texSampler, fragTexCoord);
}