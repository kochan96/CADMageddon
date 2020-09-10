//TrimTexture

#type vertex
#version 440 core

layout (location =0) in vec2 a_Position;

void main()
{
    gl_Position = vec4(2.0f*a_Position.x-1, 2.0f*a_Position.y-1.0f, 0.0f, 1.0f);
}

#type fragment
#version 440 core

layout(location = 0) out vec4 color;


void main()
{
    color = vec4(1.0f,1.0f,1.0f,1.0f);
}