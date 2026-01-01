#version 460 
#pragma shader_stage(fragment)

layout(location = 0) in vec2 I_Texcoord;
layout(location = 0) out float O_Color;

layout(set = 0, binding = 0) uniform sampler2D U_Texture;

const int U_Offset[4] = {-1, 0 , 0 ,1};
const int V_Offset[4] = { 0, -1, 1, 0};

layout(push_constant) uniform PushConstant{
    float HighThreshold;
    float LowThreshold;
};


void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(U_Texture, 0));
    float max_gradient = 0.0;
    bool IsConnect = false;

    for(int i = 0; i < 4; i++) {
        vec2 offset = vec2(U_Offset[i], V_Offset[i]) * texelSize;
        float neighbor = texture(U_Texture, I_Texcoord + offset).r;
        if(neighbor >= HighThreshold)
            IsConnect = true;
        max_gradient = max(max_gradient, neighbor);
    }

    float gradient = texture(U_Texture, I_Texcoord).r;

    if(gradient >= HighThreshold) {
        O_Color = gradient; // 强边缘
    } else if(gradient < LowThreshold) {
        O_Color = 0.0; // 弱边缘直接抑制
    } else {
        if(IsConnect) {
            O_Color = gradient; // 弱边缘与强边缘相连，保留
        } else if(gradient > max_gradient) {
            O_Color = gradient; // 局部极大值，保留
        } else {
            O_Color = 0.0; // 否则抑制
        }
    }
}
