#version 460 
#pragma shader_stage(fragment)

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec3 i_Normal;
layout(location = 2) in vec2 i_Texcoord;
layout(location = 3) in vec3 i_Tangent;
layout(location = 4) in vec3 i_Bitangent;

layout(location = 0) out vec4 o_Color;

//layout(set = 0, binding = 0) uniform sampler2D u_Texture;

layout(set = 1, binding = 1) uniform u_LightAttribute
{
    vec3 lightDir;
    vec3 lightRadiance;

    vec3 albedo;
}uLightAttributes;

layout(set = 1, binding = 2) uniform u_ActorAttribute
{
    float roughness;
    float metallic;

}uActorAttributes;

layout(push_constant) uniform PushConstants{
    vec3 cameraPos;
}pc;

const float PI = 3.14159265358979323846;


float D_GGX(vec3 NormDir, vec3 HalfDir, float Roughness);

float G_Smith(vec3 LightDir, vec3 ViewDir, vec3 NormDir, float Roughness);

vec3 F_Shlick(vec3 ViewDir, vec3 HalfDir, vec3 F0);

float G_Schlick(vec3 v, vec3 n, float k);

void main(){
    vec3 Normal = normalize(i_Normal);
    vec3 Tangent = normalize(i_Tangent);
    vec3 Bitangent = normalize(i_Bitangent);

    mat3 TBN = mat3(Tangent, Bitangent, Normal);

    vec3 ViewDir = normalize(pc.cameraPos - i_Position);
    vec3 LightDir = -normalize(uLightAttributes.lightDir);
    vec3 HalfDir = normalize(ViewDir + LightDir);
    //TODO£ºchange the normal by sample normal texture
    vec3 NormDir = Normal;


    float NdotH = max(dot(Normal, HalfDir), 0);
    float NdotL = max(dot(Normal, LightDir), 0);
    float NdotV = max(dot(Normal, ViewDir), 0);
    float LdotV = max(dot(LightDir, ViewDir), 0);

    vec3 Radiance = uLightAttributes.lightRadiance;

    vec3 F0 = vec3(0.04);
    //TODO: change the albedo by sample albedo texture
    vec3 albedo = uLightAttributes.albedo;
    F0 = mix(F0, albedo, uActorAttributes.metallic);

    //TODO: change the roughness by sample roughness texture
    float Roughness = uActorAttributes.roughness;

    float NDF = D_GGX(NormDir, HalfDir, Roughness);
    float G = G_Smith(LightDir, ViewDir, NormDir, Roughness);
    vec3 F = F_Shlick(ViewDir, HalfDir, F0);

    vec3 numerator = NDF * G * F;
    float denominator = max(4 * NdotV * NdotL, 0.001);

    vec3 BRDF = numerator / denominator;

    vec3 Lo = BRDF * Radiance * NdotL;
    vec3 color = Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    o_Color = vec4(color, 1.0);
}

vec3 F_Shlick(vec3 ViewDir, vec3 HalfDir, vec3 F0)
{
    float HdotV = max(dot(ViewDir, HalfDir), 0);
    return F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);
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

float G_Schlick(vec3 v, vec3 n, float k)
{
    float NdotV = max(dot(v,n),0);
    
    float denominator = NdotV * (1.0 - k) + k;

    return NdotV / denominator;
}

float G_Smith(vec3 LightDir, vec3 ViewDir, vec3 NormDir, float Roughness)
{
    float Alpha = Roughness * Roughness;
    float Alpha2 = Alpha * Alpha;
    float k = ((Alpha2 + 1) * (Alpha2 + 1)) / 8.0;

    return G_Schlick(LightDir, NormDir, k) * G_Schlick(ViewDir, NormDir, k);
}
