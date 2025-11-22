#version 460 
#pragma shader_stage(fragment)

layout(location = 0) in vec3 i_TexCoords;
layout(location = 0) out vec4 o_Color;

layout(set = 0,binding = 1) uniform sampler2D CylinderMap;

const vec2 InvCoef = vec2(0.1591, 0.3183);
vec2 CylinderMapUV(vec3 CubeCoord);

void main()
{    
    vec3 CubeCoord = normalize(i_TexCoords);
    vec2 UV = CylinderMapUV(CubeCoord);
    o_Color = texture(CylinderMap, UV);
}

vec2 CylinderMapUV(vec3 CubeCoord)
{
    vec2 UV = vec2(atan(CubeCoord.z, CubeCoord.x), asin(CubeCoord.y));
    UV *= InvCoef;
    UV += 0.5;
    return UV;
}

