//Color Shader

#type vertex
#version 440 core

layout (location =0) in vec3 a_Position;
layout (location =1) in vec4 a_Color;

uniform mat4 u_ViewProjectionMatrix;

out vec4 v_Color;

void main()
{
    gl_Position = u_ViewProjectionMatrix *  vec4(a_Position, 1.0f);
    v_Color = a_Color;
}

#type fragment
#version 440 core

layout(location = 0) out vec4 color;

in vec4 v_Color;

void main()
{
    color = v_Color;
}
