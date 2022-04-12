cbuffer Transforms : register(b0)
{
	float4x4 world;
	float4x4 viewProj;
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
	//float2 uv : TEXCOORD;
	//float3 norm : NORMAL;
	//float3 worldPos : WORLD_POS;
};


VSOutput main(VSInput input)
{
	VSOutput output;
	output.position = float4(input.position, 0.f);
	//output.position = mul(float4(input.position, 1.0f), mul(world, viewProj));
	//output.worldPos = mul(float4(input.position, 1.0f), world);
	output.color = input.color;
	return output;
}