Texture2D diffuseMap : DIFFUSEMAP : register(t0);
Texture2D distortionMap : DISTORTIONMAP : register(t1);
//Texture2D normalMap : NORMALMAP : register(t2);
//Texture2D opacityMap : OPACITYMAP : register(t3);
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
    int pad;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 worldPos : WORLD_POS;
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

float4 main(PSInput input) : SV_Target
{
    float2 distortion = 0.f;
    
    float2 texCoord = input.texCoord * textureScale;
    float3 lightPos = pointLightPosition;
    
    if (flags & MaterialFlag_eHasDistortion)
    {
        distortion = (distortionMap.Sample(samplerState, texCoord + texCoordoffsetDist).xy - 0.5f) / distDiv;
        //distortion /= 2;
    }
    distortion += texCoordOffset;
    
    float attenuation = 1.f;
    float3 faceNormal = normalize(input.normal);
    
    
    float4 diffuse = diffuseMap.Sample(samplerState, texCoord + distortion);
    
    float3 obToLight = lightPos.xyz - input.worldPos;

    float distance = length(obToLight);
    
    float nDotLight = saturate(dot(normalize(obToLight), faceNormal)) * pointlightStre;
    //lightindex /= distance;

    float3 lighjtIntensity = smoothstep(0.f, 0.01f, nDotLight);
    lighjtIntensity += (ambientColor * ambientStr);
    
    float3 camToOb = normalize(input.worldPos - camPos.xyz);

    float rimDot = 1 - dot(-camToOb, faceNormal);
    
    float rimamount = 0.715f;
    float rimIntensity = smoothstep(rimamount - 0.01, rimamount + 0.01, rimDot /* * lightindex*/);

    float3 final = diffuse.xyz * ((lighjtIntensity) + rimDot + (ambientColor * ambientStr)) /*+ specIntenseSmooth*/;
    
    
    
    
    return float4(final, transparency);
}