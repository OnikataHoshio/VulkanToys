#version 460 
#pragma shader_stage(fragment)

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec3 TexCoords;

layout(set = 0,binding = 1) uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}