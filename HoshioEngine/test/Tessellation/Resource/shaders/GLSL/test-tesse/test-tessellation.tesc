#version 460
#pragma shader_stage(tesscontrol)

layout(vertices = 3) out;

void main(){
	gl_TessLevelOuter[0] = 3.6f;
	gl_TessLevelOuter[1] = 4.0f;
	gl_TessLevelOuter[2] = 5.0f;
	gl_TessLevelOuter[3] = 3.0f;

	gl_TessLevelInner[0] = 3.0f;
	gl_TessLevelInner[1] = 4.0f;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}