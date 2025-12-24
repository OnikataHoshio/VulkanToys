#version 460 
#pragma shader_stage(tesseval)

layout(triangles,equal_spacing,ccw) in;

void main() {
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	float w = gl_TessCoord.z;
	
	gl_Position = u * gl_in[0].gl_Position + v * gl_in[1].gl_Position + w * gl_in[2].gl_Position;
}
