#version 460
#pragma shader_stage(vertex)

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec3 i_Normal;
layout(location = 2) in vec2 i_Texcoord;
layout(location = 3) in vec4 i_Tangent;
layout(location = 4) in vec4 i_Bitangent;

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



void main(){
	o_Position = vec3(model * vec4(i_Position, 1.0f));
	// 修正：对法向量应用模型矩阵的逆转置矩阵
	mat3 normalMatrix = mat3(transpose(inverse(mat3(model))));
	o_Normal = normalMatrix * i_Normal;
	o_Tangent = normalMatrix * vec3(i_Tangent);
	o_BiTangent = normalMatrix * vec3(i_Bitangent);
	o_Texcoord = i_Texcoord;
	gl_Position = projection * view * vec4(o_Position, 1.0f);
}
