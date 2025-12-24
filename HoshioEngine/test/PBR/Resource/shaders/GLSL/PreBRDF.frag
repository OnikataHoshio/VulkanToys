#version 460 
#pragma shader_stage(fragment)

layout(location = 0) in vec2 i_Texcoord;
layout(location = 0) out vec2 o_Color;

const float PI = 3.14159265358979323846;

float G_Smith(vec3 LightDir, vec3 ViewDir, vec3 NormDir, float Roughness);

float G_Schlick(vec3 v, vec3 n, float k);

float RadicalInverse_VdC(uint bits);

vec2 Hammersley(uint i, uint N);

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float Roughness);

vec2 IntegratedBRDF(float NdotV, float Roughness);

void main(){
	vec2 integratedBRDF = IntegratedBRDF(i_Texcoord.x, i_Texcoord.y);
	o_Color = integratedBRDF;
}

float G_Schlick(vec3 v, vec3 n, float k)
{
    float NdotV = max(dot(v,n),0);
    
    float denominator = NdotV * (1.0 - k) + k;

    return NdotV / denominator;
}

float G_Smith(vec3 LightDir, vec3 ViewDir, vec3 NormDir, float Roughness)
{
    float Alpha = Roughness;

    float k = Alpha * Alpha / 2.0;

    return G_Schlick(LightDir, NormDir, k) * G_Schlick(ViewDir, NormDir, k);
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float Roughness)
{
    float a = Roughness * Roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    vec3 H;

    H.x = sinTheta * cos(phi);
    H.y = sinTheta * sin(phi);
    H.z = cosTheta;

    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0): vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = normalize(cross(N, tangent));

    return normalize(tangent * H.x + bitangent * H.y + N * H.z);

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

vec2 IntegratedBRDF(float NdotV, float Roughness)
{
    vec3 V;
    V.x = sqrt(1.0 - NdotV * NdotV);
    V.y = 0.0;
    V.z = NdotV;

    float A = 0.0;
    float B = 0.0;

    vec3 N = vec3(0.0, 0.0, 1.0);

    const uint SampleCount = 1024u;
    for(uint i = 0u; i < SampleCount; i++)
    {
        vec2 Xi = Hammersley(i, SampleCount);
        vec3 H = ImportanceSampleGGX(Xi, N, Roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if(NdotL > 0.0)
        {
            float G = G_Smith(L, V, N, Roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(SampleCount);
    B /= float(SampleCount);

    return vec2(A, B);

}