cbuffer Transforms : register(b0)
{
	float4x4 world;
	float4x4 viewProj;
    float4x4 lightviewProj;
};

struct VSInput
{
	float3 position : POSITION;
	float3 color : COLOR;
};

struct VSOutput
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

VSOutput main(VSInput input)
{
	VSOutput output;
	output.position = mul(float4(input.position, 1.f), mul(world, viewProj));
    output.color = input.color;
	return output;
}