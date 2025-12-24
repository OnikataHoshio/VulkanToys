#version 460
#pragma shader_stage(vertex)

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec3 i_Normal;
layout(location = 2) in vec2 i_Texcoord;
layout(location = 3) in vec4 i_Tangent;
layout(location = 4) in vec4 i_Bitangent;

layout(location = 5) in vec4 i_InstanceTranslate;

layout(set = 1,binding = 0) uniform u_Transform{
    mat4 model;
    mat4 view;
    mat4 projection;
};

layout(location = 0) out vec3 o_Position;
layout(location = 1) out vec3 o_Normal;
layout(location = 2) out vec2 o_Texcoord;
layout(location = 3) out vec3 o_Tangent;
layout(location = 4) out vec3 o_BiTangent;
layout(location = 5) out flat int o_InstanceIndex;

layout(push_constant) uniform PushConstants{
    uint useKullaConty;
    vec3 cameraPos;
}pc;


void main(){
    vec3 position = vec3(model * vec4(i_Position, 1.0)) + vec3(i_InstanceTranslate);

    if(pc.useKullaConty != 0)
       o_Position = position + vec3(8.0, 0.0, 0.0);
    else
       o_Position = position - vec3(8.0, 0.0, 0.0);

    // 修正：对法向量应用模型矩阵的逆转置矩阵
    mat3 normalMatrix = mat3(transpose(inverse(mat3(model))));
    o_Normal = normalMatrix * i_Normal;
    o_Tangent = normalMatrix * vec3(i_Tangent);
    o_BiTangent = normalMatrix * vec3(i_Bitangent);
    o_Texcoord = i_Texcoord;
    o_InstanceIndex = gl_InstanceIndex;
    gl_Position = projection * view * vec4(o_Position, 1.0f);
}