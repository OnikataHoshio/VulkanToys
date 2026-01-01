#version 460 
#pragma shader_stage(fragment)

layout(location = 0) in vec2 I_Texcoord;
layout(location = 0) out vec4 O_Color;

layout(set = 0, binding = 0) uniform sampler2D U_Edge;
layout(set = 0, binding = 1) uniform sampler2D U_Slipping;
layout(set = 0, binding = 2) uniform sampler2D U_PaperTexture;
layout(set = 0, binding = 3) uniform sampler2D U_BrushTexture;

void main(){
	float Edge = 1.0 - texture(U_Edge,I_Texcoord).x;
	float Slipping = texture(U_Slipping,I_Texcoord).x;

	vec3 PaperColor = texture(U_PaperTexture, I_Texcoord).rgb;
	vec3 BrushColor = texture(U_BrushTexture, I_Texcoord).rgb;
	vec3 BaseColor = vec3(Edge * Slipping);

	vec3 BlendPaper = BaseColor * PaperColor;
	vec3 BlendBrush = (1 - BaseColor) * BrushColor;

	O_Color = vec4(BlendPaper + BlendBrush, 1.0) ;
}
