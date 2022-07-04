Texture2D diffuseMap : DIFFUSEMAP : register(t0);
Texture2D distortionMap : DISTORTIONMAP : register(t1);
Texture2D ZAToon : ZATOON : register(t2);
SamplerState samplerState : SAMPLER : register(s0);

cbuffer LightBuffer : register(b0)
{
    float3 direction;
    float p;
    float3 position;
    float ambientStr;
    float3 ambientLight;
    float atten;
    float3 pointLightColor;
    float pointlightStre;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 worldPos : WORLD_POS;
};

cbuffer MaterialBuffer : register(b1)
{
    int hasDistortion;
    float2 texCoordOffset;
    float transparency;
    float2 texCoordoffsetDist;
    int distDiv;
    int pad;
}

float4 main(PSInput input) : SV_Target
{
    float attenuation = 1.f;
    float3 faceNormal = normalize(input.normal);
    
    
    float4 diffuse = diffuseMap.Sample(samplerState, input.texCoord);
    //float dirLight = dot(faceNormal, direction);
    
    float3 obToLight = position.xyz - input.worldPos;

    float distance = length(obToLight);
    
    //float pointLightStr = saturate(dot(normalize(obToLight), faceNormal));
    //float pointLightStr = saturate(dot(normalize(obToLight), faceNormal)) * pointlightStre;
    
    //float lightindex = (dot(normalize(position), faceNormal));
    float lightindex = saturate(dot(normalize(obToLight), faceNormal)) * pointlightStre;
    lightindex /= distance;
    lightindex = clamp(lightindex, 0.01f, 0.99f);
    
    
    //light = max(light.x, 1.f);
    float cellLightStr = ZAToon.Sample(samplerState, float2(lightindex, .5f));
    
    
    
    float3 final = diffuse.xyz * cellLightStr;
    
    
    
    
    return float4(final, transparency);
}