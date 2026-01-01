#version 460 core
#pragma shader_stage(vertex)

layout(location = 0) in vec2 I_Position;
layout(location = 1) in vec2 I_Texcoord;
layout(location = 0) out vec2 O_Texcoord;

void main(){
	gl_Position = vec4(I_Position,0.0f,1.0f);
	O_Texcoord = I_Texcoord;
}