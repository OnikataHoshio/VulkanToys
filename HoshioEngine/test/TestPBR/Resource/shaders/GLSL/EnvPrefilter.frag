#version 460 
#pragma shader_stage(fragment)

layout(location = 0) in vec3 i_TexCoords;
layout(location = 0) out vec4 o_Color;

layout(set = 0,binding = 1) uniform samplerCube EnvMap;

layout(push_constant) uniform PushConstants{
    float Roughness;
}PC;

const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits);

vec2 Hammersley(uint i, uint N);

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float Roughness);

float D_GGX(vec3 NormDir, vec3 HalfDir, float Roughness);

void main()
{    
    vec3 N = normalize(i_TexCoords);
    vec3 V = N;

    const uint SampleCount = 4096u;

    vec3 color = vec3(0.0);
    float totalWeight = 0.0;

    for(uint i = 0; i < SampleCount; i++){
        vec2 Xi = Hammersley(i, SampleCount);
        vec3 H = ImportanceSampleGGX(Xi, N, PC.Roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);

        if(NdotL > 0)
        {
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);

            float D   = D_GGX(N, H, PC.Roughness);
            float pdf = (D * NdotH / (4.0 * HdotV)) + 0.0001; 

            float resolution = 512.0; // resolution of source cubemap (per face)
            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SampleCount) * pdf + 0.0001);

            float mipLevel = PC.Roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); 

            color += textureLod(EnvMap, L, mipLevel).rgb * NdotL;
            totalWeight += NdotL;
        }
    }

    color /= max(totalWeight, 0.001);
    o_Color = vec4(color, 1.0);
}

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}  

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float Roughness)
{
    float a = Roughness * Roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1-Xi.y)/(1+(a * a -1)*Xi.y));
    float sinTheta = sqrt(1-cosTheta*cosTheta);

    vec3 H;

    H.x = sinTheta * cos(phi);
    H.y = sinTheta * sin(phi);
    H.z = cosTheta;

    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0): vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = normalize(cross(N, tangent));

    return normalize(tangent * H.x + bitangent * H.y + N * H.z);

}

float D_GGX(vec3 NormDir, vec3 HalfDir, float Roughness)
{
    float Alpha = Roughness * Roughness;
    float Alpha2 = Alpha * Alpha;

    float NdotH = max(dot(NormDir, HalfDir), 0);
    float NdotH2 = NdotH * NdotH;
    float denominator = (NdotH2 * (Alpha2 - 1.0) + 1.0);
    denominator = denominator * denominator;

    return Alpha2 / (PI * denominator);
}
