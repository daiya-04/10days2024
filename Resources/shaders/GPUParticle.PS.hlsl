#include"Particle.hlsli"

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t1);
SamplerState gSampler : register(s0);


PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;
	float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    output.color = textureColor * input.color;

	return output;
}