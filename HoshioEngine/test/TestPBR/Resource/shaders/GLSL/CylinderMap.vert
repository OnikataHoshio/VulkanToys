#version 460
#pragma shader_stage(vertex)

layout (location = 0) in vec3 i_Pos;
layout (location = 0) out vec3 o_TexCoords;

layout(set = 0,binding = 0) uniform u_Transform{
    mat4 model;
    mat4 view;
    mat4 proj;
};

void main()
{
    o_TexCoords = vec3(i_Pos.x, -i_Pos.y, i_Pos.z);
    gl_Position = proj * view * model * vec4(i_Pos, 1.0);
}