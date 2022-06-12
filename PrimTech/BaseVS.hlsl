cbuffer Transforms : register(b0)
{
	float4x4 world;
	float4x4 viewProj;
	float4 color;
};

struct VSInput
{
	float3 position : POSITION;
	//float2 uv : TEXCOORD;
};

struct VSOutput
{
	float4 position : SV_POSITION;
	//float2 uv : TEXCOORD;
	//float3 norm : NORMAL;
	float3 worldPos : WORLD_POS;
	float4 color : COLOR;
};


VSOutput main(VSInput input)
{
	VSOutput output;
	output.position = mul(float4(input.position, 1.f), mul(world, viewProj));
    output.worldPos = mul(float4(input.position, 1.0f), world).xyz;
	output.color = color;
	output.color = color;
	return output;
}