#version 460
#pragma shader_stage(fragment)

layout(location = 0) in vec2 I_Texcoord;
layout(location = 0) out float O_Color;

layout(set = 0, binding = 0) uniform sampler2D U_Texture;

const mat3 SobelX = mat3(
    -1, 0, 1,
    -2, 0, 2,
    -1, 0, 1
);

const mat3 SobelY = mat3(
    -1, -2, -1,
     0,  0,  0,
     1,  2,  1
);


float CalculateIntensity(vec2 texelSize){
    float gx = 0.0;
    float gy = 0.0;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            vec2 offset = vec2(i, j) * texelSize;
            vec3 color = texture(U_Texture, I_Texcoord + offset).rgb;
            float gray = dot(color, vec3(0.299, 0.587, 0.114));
            gx += SobelX[i+1][j+1] * gray;
            gy += SobelY[i+1][j+1] * gray;
        }
    }

    return sqrt(gx * gx + gy * gy); 
}

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(U_Texture, 0));
    O_Color = CalculateIntensity(texelSize);
}
