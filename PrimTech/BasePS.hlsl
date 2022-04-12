struct PSInput
{
	float4 position : SV_POSITION;
	float3 colour : COLOR;
	float3 worldPos : WORLD_POS;
};

float4 main(PSInput input) : SV_TARGET
{
	return float4(input.colour, 1.f);
}