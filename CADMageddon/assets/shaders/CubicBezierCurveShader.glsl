//Cubic Bezier

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
layout( lines_adjacency ) in;
layout( line_strip, max_vertices=256 ) out;


vec4 bezier(vec4 p0,vec4 p1,vec4 p2,vec4 p3, float t)
{
  vec4 p12 = mix(p1,p2,t);
  vec4 p01 = mix(p0,p1,t);
  vec4 p23 = mix(p2,p3,t);

  vec4 p0112 = mix(p01,p12,t);
  vec4 p1223 = mix(p12,p23,t);

   
  return mix(p0112,p1223,t);
}

void main( )
{
    vec4 B[4];
    B[0] = gl_in[0].gl_Position;
    B[1] = gl_in[1].gl_Position;
    B[2] = gl_in[2].gl_Position;
    B[3] = gl_in[3].gl_Position;


    float dist = distance(B[0].xy / B[0].w, B[1].xy / B[1].w) + 
            distance(B[1].xy / B[1].w, B[2].xy / B[2].w) + 
            distance(B[2].xy / B[2].w, B[3].xy / B[3].w) + 0.04;
        int steps = min(int(dist * 30), 255);
        float delta = 1.0 / float(steps);
        for (int i=0; i<=steps; ++i) 
        {
            gl_Position = bezier(B[0], B[1], B[2], B[3],delta*float(i));
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
