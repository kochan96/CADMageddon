//Texture

#type vertex
#version 440 core

layout (location =0) in vec3 a_Position;
layout (location =0) in vec2 a_TextureCoordinates;

out vec2 v_TextureCoordinates;

void main()
{
    gl_Position = vec4(a_Position.x,a_Position.y, 0.0f, 1.0f);
    v_TextureCoordinates = a_TextureCoordinates;
}

#type fragment
#version 440 core

layout(location = 0) out vec4 color;

uniform sampler2D trimmingSampler;

in vec2 v_TextureCoordinates;

void main()
{
    float r = texture(trimmingSampler,v_TextureCoordinates).r;
    color = vec4(r,r,r,1.0f);
}
