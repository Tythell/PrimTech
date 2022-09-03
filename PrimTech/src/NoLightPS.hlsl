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

// Material flags
#define Flag_eNull                      (0     )
#define MaterialFlag_eHasDiffuse        (1 << 0)
#define MaterialFlag_eHasDistortion	    (1 << 1)
#define MaterialFlag_eHasNormal         (1 << 2)
#define MaterialFlag_eHasOpacity		(1 << 3)
cbuffer MaterialBuffer : register(b1)
{
    float3 diffuseColor;
    uint flags;
    
    float2 texCoordOffset;
    float transparency;
    int distDiv;
    
    float2 texCoordoffsetDist;
    float textureScale;
    int rim; // bool
    
    float4 characterLight[2];
    float3 rimColor;
    float textureScaleDist;
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
    if (flags & MaterialFlag_eHasDiffuse)
        diffuse = diffuseMap.Sample(samplerState, input.texCoord).xyz;
    
    float3 faceNormal = input.normal;
    float3 camToOb = normalize(input.worldPos - camPos.xyz);
    
    float rimDot = 0;
    if (rim == 1);
        rimDot = 1 - dot(-camToOb, faceNormal);
    
    float3 final = diffuse + (rimDot * rimColor);
    
    return float4(normalize(final), 1.f);
}