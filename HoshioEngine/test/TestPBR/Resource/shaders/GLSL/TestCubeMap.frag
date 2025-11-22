#version 460 
#pragma shader_stage(fragment)

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec3 TexCoords;

layout(set = 0,binding = 1) uniform samplerCube skybox;

void main()
{    
    vec3 color = texture(skybox, TexCoords).rgb;
    color = color/(color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}