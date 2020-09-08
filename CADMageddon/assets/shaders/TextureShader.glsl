//Texture

#type vertex
#version 440 core

layout (location =0) in vec3 a_Position;
layout (location =1) in vec2 a_TextureCoordinates;

out vec2 v_TextureCoordinates;

void main()
{
    gl_Position = vec4(a_Position.x,a_Position.y, a_Position.z, 1.0f);
    v_TextureCoordinates = a_TextureCoordinates;
}

#type fragment
#version 440 core

layout(location = 0) out vec4 color;

in vec2 v_TextureCoordinates;
uniform sampler2D sampler;

void main()
{
    vec3 textureColor = texture(sampler,v_TextureCoordinates).rgb;
    //color = vec4(textureColor,.5f);

    color = vec4(textureColor,1.0f);
}
