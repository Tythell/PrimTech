Texture2D diffuseMap : DIFFUSEMAP : register(t0);
SamplerState samplerState : SAMPLER : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 worldPos : WORLD_POS;
};

float4 main(PSInput input) : SV_Target
{
    float3 diffuse = diffuseMap.Sample(samplerState, input.texCoord);
    
    return float4(diffuse, 1.f);
}