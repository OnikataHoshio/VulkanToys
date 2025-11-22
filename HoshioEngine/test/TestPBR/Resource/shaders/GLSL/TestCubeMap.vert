#version 460
#pragma shader_stage(vertex)
layout (location = 0) in vec3 aPos;
layout (location = 0) out vec3 TexCoords;

layout(set = 0,binding = 0) uniform u_Transform{
    mat4 model;
    mat4 view;
    mat4 proj;
};

void main()
{
    TexCoords = vec3(aPos.x, aPos.y, -aPos.z);
    gl_Position = proj * view * model * vec4(aPos, 1.0);
}