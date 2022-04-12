struct VSInput
{
	float2 position : POSITION;
	float3 color : COLOR;
};

struct VSOutput
{
	float4 position : SV_Position;
	float3 color : COLOR;
	//float2 uv : TEXCOORD;
	//float3 norm : NORMAL;
	//float3 worldPos : WORLD_POS;
};

VSOutput main(VSInput input)
{
	VSOutput output;
	output.position = float4(input.position, 0.f, 1.f);
	output.color = input.color;
	return output;
}