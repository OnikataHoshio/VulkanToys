#version 460 
#pragma shader_stage(fragment)

layout(location = 0) in vec2 I_Texcoord;
layout(location = 0) out float O_Color;

layout(set = 0, binding = 0) uniform sampler2D U_Texture;
layout(set = 0, binding = 1) uniform sampler2D U_EdgeTex;

layout(push_constant) uniform PushConstant{
    float radius_factor;
};

float rgb2luminance(vec3 color) 
{ 
	return dot(color, vec3(0.299, 0.587, 0.114)); 
}

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(U_EdgeTex, 0));

    float luminance = rgb2luminance(texture(U_Texture, I_Texcoord).rgb);
    int radius = int((1.0 - luminance) * radius_factor); // 亮度越低，半径越大

    float minVal = 1.0;
    for(int i=-radius; i<=radius; i++) {
        for(int j=-radius; j<=radius; j++) {
            vec2 offset = vec2(i,j) * texelSize;
            float val = texture(U_EdgeTex, I_Texcoord + offset).r;
            minVal = min(minVal, val);
        }
    }

    O_Color = minVal; // 收缩结果
}
