#version 460 
#pragma shader_stage(fragment)

layout(location = 0) in vec2 i_Texcoord;
layout(location = 0) out float o_Color;

layout(set = 0, binding = 0) uniform sampler2D u_Texture;

float ComputeBRDF(float F0, vec2 precomputeBRDF)
{
	return F0 * precomputeBRDF.x + precomputeBRDF.y;
}

float IntegrateEavg()
{
    const uint SampleCount = 1024u;

    float roughness = i_Texcoord.y;
    float F_0 = i_Texcoord.x;
    float E_avg = 0.0;

    for (uint i = 0u; i < SampleCount; ++i)
    {
        float NdotV = (float(i) + 0.5) / float(SampleCount);

        vec2 precomputeBRDF = texture(u_Texture, vec2(NdotV, roughness)).rg;

        float Ed = ComputeBRDF(F_0, precomputeBRDF); // = F0*A + B

        E_avg += Ed * NdotV;
    }

    E_avg = 2.0 * (E_avg / float(SampleCount));
    return E_avg;
}

void main(){
	vec2 precomputeBRDF = texture(u_Texture, i_Texcoord).rg;
	//float OneMinusE = 1 - ComputeBRDF(PC.F0, precomputeBRDF);
    //float E = 1 - OneMinusE;
	float E_avg = IntegrateEavg();
	o_Color = E_avg;
}