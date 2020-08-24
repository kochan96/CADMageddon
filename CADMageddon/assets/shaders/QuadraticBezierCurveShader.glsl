//Flat Color Shader

#type vertex
#version 440 core

layout (location =0) in vec3 a_Position;
uniform mat4 u_ViewProjectionMatrix;


void main()
{
    gl_Position = u_ViewProjectionMatrix*vec4(a_Position, 1.0f);
}

#type geometry
#version 440 core
layout( triangles ) in;
layout( line_strip, max_vertices=256 ) out;

vec4 bezier(vec4 p0,vec4 p1,vec4 p2, float t)
{
  vec4 p01 = mix(p0,p1,t);
  vec4 p12 = mix(p1,p2,t);
  vec4 p0112 = mix(p01,p12,t);
   
  return p0112;
}

void main( )
{
    float dist = distance(gl_in[0].gl_Position.xy, gl_in[1].gl_Position.xy) + distance(gl_in[1].gl_Position.xy, gl_in[2].gl_Position.xy);
    int steps = min(int(dist * 10), 256);
    float delta = 1.0 / float(steps);
    for (int i=0; i<=steps; ++i)
    {
        gl_Position = bezier(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position,delta * float(i));
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
