//Torus Shader

#type vertex
#version 440 core

layout (location =0) in vec3 a_Position;
layout (location =1) in vec2 a_TextureCoordinates;

uniform mat4 u_ViewProjectionMatrix;
uniform mat4 u_ModelMatrix;

out vec2 v_TextureCoordinates;

void main()
{
    gl_Position = u_ViewProjectionMatrix * u_ModelMatrix * vec4(a_Position, 1.0f);
    v_TextureCoordinates = a_TextureCoordinates.xy;
}

#type fragment
#version 440 core

layout(location = 0) out vec4 color;

in vec2 v_TextureCoordinates;

uniform vec4 u_Color;
uniform bool isTrimmed;
uniform bool reverseTrimming;

uniform sampler2D trimmingSampler;

void main()
{

    if(isTrimmed)
    {
        float alfa = texture(trimmingSampler,v_TextureCoordinates).r;
        if(reverseTrimming)
        {
           alfa = 1.0f - alfa; 
        }

        if(alfa <1.0f)
            discard;
    }

    color = u_Color;
}
