#version 460 
#pragma shader_stage(fragment)

layout(location = 0) in vec2 I_Texcoord;
layout(location = 0) out float O_Color;

layout(set = 0, binding = 0) uniform sampler2D U_Texture;
layout(set = 0, binding = 1) uniform sampler2D U_BlueNoise;

layout(push_constant) uniform PushConstant{
    float uv_scale;
	float luminance_scale;
};

float rgb2luminance(vec3 color) 
{ 
	return dot(color, vec3(0.299, 0.587, 0.114)) * luminance_scale; 
}

void main(){
	vec2 size = vec2(textureSize(U_Texture, 0));

	vec2 aspect = vec2(size.x / max(size.x, size.y), size.y / max(size.x, size.y));
	float Luminance = rgb2luminance(texture(U_Texture, I_Texcoord).rgb);
	float NoiseValue = texture(U_BlueNoise, I_Texcoord * aspect * uv_scale).r;



	if(Luminance > NoiseValue)
		O_Color = 1.0;
	else
		O_Color = 0.0;
}
