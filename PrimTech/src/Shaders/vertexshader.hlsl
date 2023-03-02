cbuffer TransformBuffer : register(b0)
{
    float4x4 world;
    float4x4 viewProj;
    float4x4 lightviewProj;
};

struct VSInput
{
    float3 localPosition : POSITION;
    float2 texCoord : TEXCOORD;
    float3 localNormal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float3 vcolor : COLOR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 worldpos : WORLD_POS;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 clipSpace : CLIPSPACE;
    float3 vcolor : COLOR;
};

VSOutput main(VSInput input)
{   
    VSOutput output;
    output.position = mul(float4(input.localPosition.xyz, 1.f), mul(world, viewProj));
    output.texCoord = input.texCoord;
    output.normal = normalize(mul(float4(input.localNormal, 0.f), world));
    output.worldpos = mul(float4(input.localPosition.xyz, 1.f), world).xyz;
    output.tangent = mul(float4(input.tangent, 0.f), world).xyz;
    output.bitangent = mul(float4(input.bitangent, 0.f), world).xyz;
    output.clipSpace = mul(float4(input.localPosition, 1.f), mul(world, lightviewProj));
    output.vcolor = input.vcolor;
    return output;
}