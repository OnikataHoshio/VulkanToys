#version 460 
#pragma shader_stage(fragment)

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec3 i_Normal;
layout(location = 2) in vec2 i_Texcoord;
layout(location = 3) in vec3 i_Tangent;
layout(location = 4) in vec3 i_Bitangent;
layout(location = 5) in flat int i_InstanceIndex;

layout(location = 0) out vec4 o_Color;

layout(set = 0, binding = 0) uniform sampler2D u_PreBRDF;
layout(set = 0, binding = 1) uniform sampler2D u_KullaConty;
layout(set = 0, binding = 2) uniform samplerCube u_EnvPrefilter;

layout(set = 1, binding = 1) uniform u_LightAttribute
{
    vec3 lightDir;
    vec3 lightRadiance;

    vec3 albedo;
}uLightAttributes;

layout(set = 1, binding = 2) uniform u_ActorAttribute
{
    float roughness[25];
    float metallic[25];
}uActorAttributes;

layout(push_constant) uniform PushConstants{
    uint useKullaConty;
    vec3 cameraPos;
}pc;

const float PI = 3.14159265358979323846;


float D_GGX(vec3 NormDir, vec3 HalfDir, float Roughness);

float G_Smith(vec3 LightDir, vec3 ViewDir, vec3 NormDir, float Roughness);

vec3 F_Shlick(vec3 ViewDir, vec3 HalfDir, vec3 F0);

float G_Schlick(vec3 v, vec3 n, float k);

vec3 AverageFresnel(vec3 r, vec3 g);

vec3 MultiScatterBRDF(float NdotL, float NdotV, vec3 Albedo, vec3 F0, float Roughness);

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
    F0 = mix(F0, albedo, uActorAttributes.metallic[i_InstanceIndex]);

    //TODO: change the roughness by sample roughness texture
    float Roughness = uActorAttributes.roughness[i_InstanceIndex];

    float NDF = D_GGX(NormDir, HalfDir, Roughness);
    float G = G_Smith(LightDir, ViewDir, NormDir, Roughness);
    vec3 F = F_Shlick(ViewDir, HalfDir, F0);

    vec3 numerator = NDF * G * F;
    float denominator = max(4 * NdotV * NdotL, 0.001);

    vec3 BRDF = numerator / denominator;

    vec3 Lo = BRDF * Radiance * NdotL;
    vec3 color = Lo;

    // IBL
    vec2 preBRDF = texture(u_PreBRDF, vec2(NdotV, Roughness)).rg;

    vec3 R = normalize(2.0 * dot(ViewDir, NormDir) * NormDir - ViewDir);
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 envPrefilter = textureLod(u_EnvPrefilter, R, Roughness * MAX_REFLECTION_LOD).rgb;

    color += envPrefilter * (F0 * preBRDF.x + preBRDF.y);

    if(pc.useKullaConty != 0)
       color += MultiScatterBRDF(NdotL, NdotV, albedo, F0, Roughness);

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    o_Color = vec4(color , 1.0);
}

vec3 AverageFresnel(vec3 r, vec3 g)
{
    return vec3(0.087237) + 0.0230685*g - 0.0864902*g*g + 0.0774594*g*g*g
           + 0.782654*r - 0.136432*r*r + 0.278708*r*r*r
           + 0.19744*g*r + 0.0360605*g*g*r - 0.2586*g*r*r;
}

vec3 MultiScatterBRDF(float NdotL, float NdotV, vec3 Albedo, vec3 F0, float Roughness)
{
  vec3 albedo = pow(Albedo, vec3(2.2));

  vec2 preBRDF = texture(u_PreBRDF, vec2(NdotV, Roughness)).rg;
  vec3 E_i = F0 * preBRDF.x + preBRDF.y;
  NdotL = 0.8 * NdotL + 0.2;
  preBRDF = texture(u_PreBRDF, vec2(NdotL, Roughness)).rg;
  vec3 E_o = F0 * preBRDF.x + preBRDF.y;

  vec3 E_avg;
  E_avg.x = texture(u_KullaConty, vec2(F0.x, Roughness)).r;
  E_avg.y = texture(u_KullaConty, vec2(F0.y, Roughness)).r;
  E_avg.z = texture(u_KullaConty, vec2(F0.z, Roughness)).r;

  // copper
  vec3 edgetint = F0;
  vec3 F_avg = AverageFresnel(albedo, edgetint);
  
  // TODO: To calculate fms and missing energy here
  vec3 F_add = (F_avg * E_avg) / (1.0 - F_avg * (1 - E_avg));

  vec3 F_ms = ((1.0 - E_o) * (1.0 - E_i))/(PI * (1.0 - E_avg));

  return F_add * F_ms;
  
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
