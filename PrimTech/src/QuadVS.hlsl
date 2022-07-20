cbuffer Transforms : register(b0)
{
	float4x4 world;
	float4x4 viewProj;
};

struct VSInput
{
	float3 position : POSITION;
};

struct VSOutput
{
	float4 position : SV_POSITION;
	float3 worldPos : WORLD_POS;
};


VSOutput main(VSInput input)
{
	VSOutput output;
	output.position = mul(float4(input.position, 1.f), mul(world, viewProj));
    output.worldPos = mul(float4(input.position, 1.f), world).xyz;
	return output;
}