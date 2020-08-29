//Cubic Bezier

#type vertex
#version 440 core

layout (location =0) in float a_Position;

void main()
{
    gl_Position = vec4(a_Position,0.0f,0.0f, 1.0f);
}

#type geometry
#version 440 core
layout( points ) in;
layout( line_strip, max_vertices=256 ) out;

uniform vec3 gregoryPoints[20];
uniform bool isForward;
uniform mat4 u_ViewProjectionMatrix;

vec4 BernsteinBasis(float t)
{
    float invT = 1.0f - t;

    return vec4( invT * invT * invT,
                   3.0f * t * invT * invT,
                   3.0f * t * t * invT,
                   t * t * t );
}

void main()
{

    float x = gl_in[0].gl_Position.x, u, v;
    vec4 uCoord, vCoord;// = toBernstein3(u);
    if (isForward) 
    {
        v = x;   
        vCoord = BernsteinBasis(x);
    }
    else {
        u = x;
        uCoord = BernsteinBasis(x);
    }

    vec3 mat[4][4];
    mat[0][0] = gregoryPoints[0];
    mat[0][1] = gregoryPoints[2];
    mat[0][2] = gregoryPoints[6];
    mat[0][3] = gregoryPoints[5];
    mat[1][3] = gregoryPoints[7];
    mat[2][3] = gregoryPoints[11];
    mat[3][3] = gregoryPoints[10];
    mat[3][2] = gregoryPoints[12];
    mat[3][1] = gregoryPoints[16];
    mat[3][0] = gregoryPoints[15];
    mat[2][0] = gregoryPoints[17];
    mat[1][0] = gregoryPoints[1];

    vec4 corner[4];
    corner[0] = u_ViewProjectionMatrix * vec4(gregoryPoints[0], 1);
    corner[1] = u_ViewProjectionMatrix * vec4(gregoryPoints[5], 1);
    corner[2] = u_ViewProjectionMatrix * vec4(gregoryPoints[10], 1);
    corner[3] = u_ViewProjectionMatrix * vec4(gregoryPoints[15], 1);
    float dist = distance(corner[0].xy / corner[0].w, corner[1].xy / corner[1].w) + 
        distance(corner[1].xy / corner[1].w, corner[2].xy / corner[2].w) + 
        distance(corner[2].xy / corner[2].w, corner[3].xy / corner[3].w) +
        distance(corner[3].xy / corner[3].w, corner[0].xy / corner[0].w);
    int steps = min(int(dist * 20) + 1, 255);
    float delta = 1.0 / float(steps);
    for (int i = 0; i <= steps; ++i) 
    {
        if (isForward) 
        {
            u = i * delta;
            uCoord = BernsteinBasis(u);
        }
        else 
        {
            v = i * delta;
            vCoord = BernsteinBasis(v);
        }

        if(u != 0 && u != 1 && v != 0 && v != 1) {
            mat[1][1] = (u * gregoryPoints[4] + v * gregoryPoints[3]) / (u + v);
            mat[1][2] = ((1 - u) * gregoryPoints[8] + v * gregoryPoints[9]) / (1 - u + v);
            mat[2][1] = (u * gregoryPoints[18] + (1 - v) * gregoryPoints[19]) / (1 + u - v);
            mat[2][2] = ((1 - u) * gregoryPoints[14] + (1 - v) * gregoryPoints[13]) / (2 - u - v);
        }

        vec4 Pos = vec4(0, 0, 0, 1);
        for (int j = 0; j < 4; j++)
            for(int k = 0; k < 4; k++){
                Pos.xyz += mat[j][k] * uCoord[k] * vCoord[j];
            }
        gl_Position = u_ViewProjectionMatrix * Pos;
        EmitVertex();
    }
}


#type fragment
#version 440 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main()
{
    color = u_Color;
}
