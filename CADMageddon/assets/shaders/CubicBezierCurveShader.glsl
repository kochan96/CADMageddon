//Flat Color Shader

#type vertex
#version 440 core

layout (location =0) in vec3 a_Position;

void main()
{
    gl_Position = vec4(a_Position, 1.0f);
}

#type geometry
#version 440 core
layout( lines_adjacency ) in;
layout( line_strip, max_vertices=256 ) out;

uniform mat4 u_ViewProjectionMatrix;
uniform float u_SubdivisionCount;

vec3 bezier(vec3 p0,vec3 p1,vec3 p2,vec3 p3, float t)
{
  vec3 p01 = mix(p0,p1,t);
  vec3 p12 = mix(p1,p2,t);
  vec3 p23 = mix(p2,p3,t);

  vec3 p0112 = mix(p01,p12,t);
  vec3 p1223 = mix(p12,p23,t);

   
  return mix(p0112,p1223,t);
}

void main( )
{
    float deltaT = 1.0f/u_SubdivisionCount;

    for(int i=0;i<=u_SubdivisionCount;i++)
    {
        float t = i*deltaT;
        gl_Position = u_ViewProjectionMatrix * vec4(bezier(gl_in[0].gl_Position.xyz,gl_in[1].gl_Position.xyz,gl_in[2].gl_Position.xyz,gl_in[3].gl_Position.xyz,t),1.0f);
        EmitVertex();
    }

    EndPrimitive();
}


#type fragment
#version 440 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main()
{
    color = u_Color;
}
