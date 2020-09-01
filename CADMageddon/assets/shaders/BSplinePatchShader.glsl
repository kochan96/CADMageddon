//Bezier Patch

#type vertex
#version 440 core

layout (location =0) in vec3 a_Position;

void main()
{
    gl_Position = vec4(a_Position, 1.0f);
}

#type tessControl
#version 440 core
layout (vertices = 16) out;

uniform float u_SubdivisionCount;

void main()
{
    gl_TessLevelOuter[0] = u_SubdivisionCount;
    gl_TessLevelOuter[1] = 64;

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}

#type tessEval
#version 440 core
layout (isolines, equal_spacing, cw) in;

uniform mat4 u_ViewProjectionMatrix;

float intval = 1.0;

float spline0(float t, float ti)
{
    if(ti>t && ti-intval<=t)
        return 1;
    else
        return 0;
}

float spline1(float t, float ti)
{
    float val1 = spline0(t, ti) * (t - ti + intval);
    float val2 = spline0(t, ti + intval) * (ti + intval - t);
    return (val1 + val2) / intval;
}
float spline2(float t, float ti)
{
    float val1 = spline1(t, ti) * (t - ti + intval);
    float val2 = spline1(t, ti + intval) * (ti + 2 * intval - t);
    return (val1 + val2) / (2 * intval);
}
float spline3(float t, float ti)
{
    float val1 = spline2(t, ti) * (t - ti + intval);
    float val2 = spline2(t, ti + intval) * (ti + 3 * intval - t);
    return (val1 + val2) / (3 * intval);
}

vec4 SplineBasis(float t)
{
    return vec4(spline3(t, 1 - 3 * intval), spline3(t, 1 - 2 * intval), 
    spline3(t, 1 -  intval), spline3(t, 1));
}

vec3 CubicSplineSum(vec4 bezpatch[16], vec4 basisU, vec4 basisV)
{
    vec3 sum = vec3(0.0f, 0.0f, 0.0f);
    sum  = basisU.x * (basisV.x*bezpatch[0].xyz  + basisV.y*bezpatch[4].xyz  + basisV.z*bezpatch[8].xyz  + basisV.w*bezpatch[12].xyz);
    sum += basisU.y * (basisV.x*bezpatch[1].xyz  + basisV.y*bezpatch[5].xyz  + basisV.z*bezpatch[9].xyz  + basisV.w*bezpatch[13].xyz);
    sum += basisU.z * (basisV.x*bezpatch[2].xyz  + basisV.y*bezpatch[6].xyz  + basisV.z*bezpatch[10].xyz + basisV.w*bezpatch[14].xyz);
    sum += basisU.w * (basisV.x*bezpatch[3].xyz + basisV.y*bezpatch[7].xyz + basisV.z*bezpatch[11].xyz + basisV.w*bezpatch[15].xyz);

    return sum;
}

void main ()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 bezpatch[16];
    bezpatch[0] = gl_in[0].gl_Position;
    bezpatch[1] = gl_in[1].gl_Position;
    bezpatch[2] = gl_in[2].gl_Position;
    bezpatch[3] = gl_in[3].gl_Position;
    
    bezpatch[4] = gl_in[4].gl_Position;
    bezpatch[5] = gl_in[5].gl_Position;
    bezpatch[6] = gl_in[6].gl_Position;
    bezpatch[7] = gl_in[7].gl_Position;
    
    bezpatch[8] = gl_in[8].gl_Position;
    bezpatch[9] = gl_in[9].gl_Position;
    bezpatch[10] = gl_in[10].gl_Position;
    bezpatch[11] = gl_in[11].gl_Position;
    
    bezpatch[12] = gl_in[12].gl_Position;
    bezpatch[13] = gl_in[13].gl_Position;
    bezpatch[14] = gl_in[14].gl_Position;
    bezpatch[15] = gl_in[15].gl_Position;

    vec4 basisU = SplineBasis(u);
	vec4 basisV = SplineBasis(v);

    vec3 pos  = CubicSplineSum(bezpatch, basisU, basisV);

    gl_Position = u_ViewProjectionMatrix * vec4(pos,1.0f);
}


#type fragment
#version 440 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main()
{
    color = u_Color;
}