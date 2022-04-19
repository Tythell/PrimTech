Texture2D albedo : TEXCOORD : register(t0);
SamplerState samplerState : SAMPLER : register(s0);

struct PSInput
{
	float4 position : SV_POSITION;
	//float3 colour : COLOR;
	float2 uv : TEXCOORD;
	float3 worldPos : WORLD_POS;
	float4 color : COLOR;
};

float4 main(PSInput input) : SV_TARGET
{
	float3 albedoColor = albedo.Sample(samplerState, input.uv);
	
	
	return float4(input.color.xyz, 1.f);
}