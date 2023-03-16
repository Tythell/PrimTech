
struct PSInput
{
	float4 position : SV_POSITION;
	//float2 uv : TEXCOORD;
    float3 color : COLOR;
};

float4 main(PSInput input) : SV_TARGET
{
	//float3 albedoColor = albedo.Sample(samplerState, input.uv);
	
	
	return float4(input.color, 1.f);
}