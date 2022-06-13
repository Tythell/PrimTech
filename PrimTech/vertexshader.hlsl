cbuffer TransformBuffer : register(b0)
{
    matrix world;
    matrix viewProj;
};

struct VSInput
{
    float3 localPosition : POSITION;
    float3 localNormal : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 worldpos : WORLD_POS;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.position = mul(float4(input.localPosition, 1.f), mul(world, viewProj));
    output.normal = normalize(mul(input.localNormal, (float3x3) world));
    output.texCoord = input.texCoord;
    output.worldpos = mul(float4(input.localPosition, 1.f), world).xyz;
    return output;
}