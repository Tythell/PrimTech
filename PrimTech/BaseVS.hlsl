struct VSInput
{
	float4 position : POSITION;
	float2 uv : TEXCOORD;
	float3 norm : NORMAL;
};

struct VSOutput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 norm : NORMAL;
	
};