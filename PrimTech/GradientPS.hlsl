Texture2D diffuseMap : DIFFUSEMAP : register(t0);
Texture2D distortionMap : DISTORTIONMAP : register(t1);
Texture2D ZAToon : ZATOON : register(t2);
SamplerState samplerState : SAMPLER : register(s0);

cbuffer LightBuffer : register(b0)
{
    float3 direction;
    float specularInstensity;
    float3 position;
    float ambientStr;
    float3 ambientLight;
    float atten;
    float3 pointLightColor;
    float pointlightStre;
    float3 forwardDir;
    int pad;
    float4 camPos;
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
    float textureScale;
}

float4 main(PSInput input) : SV_Target
{
    float2 distortion = 0.f;
    
    float2 texCoord = input.texCoord * textureScale;
    float3 lightPos = position;
    //float2 texCoord = input.texCoord;
    //float2 tileOffset = float2(1.0f, 1.f) / textureScale;
    //texCoord += tileOffset;
    //texCoord *= textureScale;
    //texCoord -= tileOffset;
    //texCoord += tileOffset;
    
    if (hasDistortion)
    {
        distortion = (distortionMap.Sample(samplerState, texCoord + texCoordoffsetDist).xy - 0.5f) / distDiv;
        //distortion /= 2;
    }
    distortion += texCoordOffset;
    
    float attenuation = 1.f;
    float3 faceNormal = normalize(input.normal);
    
    
    float4 diffuse = diffuseMap.Sample(samplerState, texCoord + distortion);
    //float dirLight = dot(faceNormal, direction);
    
    float3 obToLight = lightPos.xyz - input.worldPos;

    float distance = length(obToLight);
    
    //float pointLightStr = saturate(dot(normalize(obToLight), faceNormal));
    //float pointLightStr = saturate(dot(normalize(obToLight), faceNormal)) * pointlightStre;
    
    //float lightindex = (dot(normalize(position), faceNormal));
    float lightindex = saturate(dot(normalize(obToLight), faceNormal)) * pointlightStre;
    //lightindex /= distance;
    
    
    // Uses to calculated value as index on a lookup-table stored in a texture
    //lightindex = clamp(lightindex, 0.01f, 0.99f);
    //float cellLightStr = ZAToon.Sample(samplerState, float2(lightindex, .5f));
    float lighjtIntensity = smoothstep(0.f, 0.01f, lightindex);
    lighjtIntensity += (ambientLight * ambientStr);
    
    float3 camToOb = normalize(input.worldPos - camPos.xyz);

    //float3 halfVector = normalize(lightPos - viewDir);
    //float nDotH = dot(faceNormal, halfVector);
    
    //float specIntens = pow(nDotH * pointlightStre, specularInstensity);
    
    //float specIntenseSmooth = smoothstep(0.f, 0.01, specIntens);
    
    float rimDot = 1 - dot(-camToOb, faceNormal);
    
    float rimamount = 0.715f;
    float rimIntensity = smoothstep(rimamount - 0.01, rimamount + 0.01, rimDot * lightindex);
    
    //rimIntensity = 
    
    
    //rimIntensity *= saturate(lighjtIntensity);
    
    //float3 final = diffuse.xyz * cellLightStr;
    float3 final = diffuse.xyz * ((lighjtIntensity) + rimIntensity + (ambientLight * ambientStr)) /*+ specIntenseSmooth*/;
    
    
    
    
    return float4(final, transparency);
}