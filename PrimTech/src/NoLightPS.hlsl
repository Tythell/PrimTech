Texture2D diffuseMap : DIFFUSEMAP : register(t1);
SamplerState samplerState : SAMPLER : register(s0);

cbuffer LightBuffer : register(b0)
{
    float3 direction;
    float specularInstensity;
    float3 pointLightPosition;
    float ambientStr;
    float3 ambientColor;
    float atten;
    float3 pointLightColor;
    float pointlightStre;
    float3 camPos;
    float lightDistance;
    float3 shadowDir;
    float pad;
};

cbuffer MaterialBuffer : register(b1)
{
    int hasDistortion;
    float2 texCoordOffset;
    float transparency;
    float2 texCoordoffsetDist;
    int distDiv;
    float textureScale;
    float3 rimColor;
    int rim;
    int hasNormal;
    int LH;
    int hasOpacityMap;
    float textureScaleDist;
    float4 characterLight[2];
    float3 diffuseColor;
    int hasDiffuse;
}

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 worldPos : WORLD_POS;
};

float4 main(PSInput input) : SV_Target
{
    float3 diffuse = diffuseColor;
    if (hasDiffuse)
        diffuse = diffuseMap.Sample(samplerState, input.texCoord).xyz;
    
    float3 faceNormal = input.normal;
    float3 camToOb = normalize(input.worldPos - camPos.xyz);
    
    float rimDot = 0;
    if (rim == 1);
        rimDot = 1 - dot(-camToOb, faceNormal);
    
    float3 final = diffuse + (rimDot * rimColor);
    
    return float4(normalize(final), 1.f);
}