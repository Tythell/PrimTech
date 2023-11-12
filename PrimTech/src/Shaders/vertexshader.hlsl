cbuffer TransformBuffer : register(b0)
{
    float4x4 world;
    float4x4 projView;
    float4x4 lightviewProj;
};

struct VSInput
{
    float3 localPosition : POSITION;
    float2 texCoord : TEXCOORD;
    float3 localNormal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4x4 instWorld : INSTWORLD;
    //float3 instWorld : INSTWORLD;
    //float3 instWorld1 : INSTWORLD1;
    //float3 instWorld2 : INSTWORLD2;
    //float3 instWorld3 : INSTWORLD3;
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
    //float3 vcolor : COLOR;
};

VSOutput main(VSInput input)
{   
    VSOutput output;
    // Copy matrix from instance buffer
    
    // Not all drawcalls will be instanced
    // When drawing instanced, worldMatrix will be identity, and when drawing noninstanced geometry inst will be identity,
    // this way, switching vertex shader won't be neccesary
    float4x4 finalWorld = mul(input.instWorld, world);
    
    //output.position = mul(float4(input.localPosition.xyz, 1.f), mul(world, viewProj));
    
    //matrix pvw = ;
    
    output.position = mul(mul(projView, finalWorld), float4(input.localPosition.xyz, 1.f));
    output.texCoord = input.texCoord;
    
    output.normal = normalize(mul(finalWorld, float4(input.localNormal, 0.f)));
    output.worldpos = mul(finalWorld, float4(input.localPosition.xyz, 1.f)).xyz;
    output.tangent = mul(float4(input.tangent, 0.f), finalWorld).xyz;
    output.bitangent = mul(float4(input.bitangent, 0.f), finalWorld).xyz;
    output.clipSpace = mul(mul(lightviewProj, finalWorld), float4(input.localPosition, 1.f));
    //output.vcolor = input.vcolor;
    return output;
}