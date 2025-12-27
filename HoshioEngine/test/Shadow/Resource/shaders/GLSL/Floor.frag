#version 460 
#pragma shader_stage(fragment)

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec3 i_Normal;

layout(location = 0) out vec4 o_Color;

layout(set = 1, binding = 1) uniform u_Attributes
{
    vec3 CameraPos;

    vec3 PointLightPos;
    vec3 I_p;

    vec3 SunLightDir;
    vec3 I_s;

    float Shininess;
}uAttributes;


vec3 CalIlluminate(vec3 texColor, vec3 L, vec3 N, vec3 H, vec3 I, float attenuation);

vec3 CalFloorColor();

void main(){
    vec3 N = normalize(i_Normal);
    vec3 V = normalize(uAttributes.CameraPos - i_Position);

    vec3 texColor = CalFloorColor();

    vec3 color = 0.05 * texColor;

    // 点光源计算
    vec3 L = normalize(uAttributes.PointLightPos - i_Position);
    vec3 H = normalize(L + V);
    vec3 dist = uAttributes.PointLightPos - i_Position;
    float dist2 = dot(dist, dist);
    // 增加最小距离限制，避免衰减过大
    float attenuation = 1.0 / max(dist2, 0.1); 
    color += CalIlluminate(texColor, L, N, H, uAttributes.I_p, attenuation);

    // 平行光计算
    L = normalize(-uAttributes.SunLightDir);
    H = normalize(L + V);
    color += CalIlluminate(texColor, L, N, H, uAttributes.I_s, 1.0);

    // 补充Alpha通道为纹理的Alpha值

    o_Color = vec4(color, 1.0);
}

vec3 CalIlluminate(vec3 texColor, vec3 L, vec3 N, vec3 H, vec3 I, float attenuation)
{
    float Shininess = uAttributes.Shininess;

    vec3 color = vec3(0.0);
    // 漫反射
    float NdotL = max(dot(N, L), 0.0);
    color += texColor * attenuation * I * NdotL;

    // 镜面反射
    float NdotH = max(dot(N, H), 0.0);
    color += vec3(0.3) * attenuation * I * pow(NdotH, Shininess);

    return color;
}

vec3 CalFloorColor(){
    int x = int(floor(i_Position.x));
    int y = int(floor(i_Position.z));

    bool isOdd = ((x + y) % 2 != 0);

    if(isOdd)
        return vec3(0.4f);
    else
        return vec3(1.0f);
}