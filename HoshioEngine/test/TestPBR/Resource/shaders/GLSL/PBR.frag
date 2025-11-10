#version 460 
#pragma shader_stage(fragment)

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec3 i_Normal;
layout(location = 2) in vec2 i_Texcoord;

layout(location = 0) out vec4 o_Color;

//layout(set = 0, binding = 0) uniform sampler2D u_Texture;

vec3 hsv2rgb(vec3 c)
{
    vec3 rgb = clamp( abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0),
                              6.0) - 3.0) - 1.0,
                      0.0,
                      1.0 );
    rgb = rgb * rgb * (3.0 - 2.0 * rgb); // 平滑插值（可选）
    return c.z * mix(vec3(1.0), rgb, c.y);
}

void main(){
	o_Color = vec4(hsv2rgb(vec3(i_Texcoord.x,1.0, (i_Texcoord.y + 0.5)/1.5)), 1.0);
}