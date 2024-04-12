#version 450

vec2 g_aPositions[] =
{
    vec2(0.0f, -0.5f),
    vec2(0.5f, 0.5f),
    vec2(-0.5f, 0.5f)
};

vec3 g_aColors[] =
{
    vec3(1.0f, 0.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 0.0f, 1.0f)
};

layout(location = 0) out vec3 fragmentColor;

void main() 
{
    gl_Position = vec4(g_aPositions[gl_VertexIndex], 0.0f, 1.0f);
    fragmentColor = g_aColors[gl_VertexIndex];
}