//Quad Shader

#type vertex
#version 440 core

layout (location =0) in vec2 a_Position;
layout (location =1) in vec2 a_TextureCoord;

out vec2 v_TextureCoord;

void main()
{
    v_TextureCoord = a_TextureCoord;
    gl_Position = vec4(a_Position, 0.0f, 1.0f);
}

#type fragment
#version 440 core

layout(location = 0) out vec4 color;

in vec2 v_TextureCoord;

uniform vec4 u_Color;
uniform sampler2D leftEyeFrame;
uniform sampler2D rightEyeFrame;


void main()
{
    vec3 colorLeft = texture(leftEyeFrame, v_TextureCoord).rgb;
    vec3 colorRight = texture(rightEyeFrame, v_TextureCoord).rgb;
    color = vec4(colorLeft + colorRight, 1.0f);
}
