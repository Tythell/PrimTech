Texture2D ZAToon : ZATOON : register(t0);
Texture2D diffuseMap : DIFFUSEMAP : register(t1);
Texture2D distortionMap : DISTORTIONMAP : register(t2);
Texture2D normalMap : NORMALMAP : register(t3);
Texture2D opacityMap : NORMALMAP : register(t4);
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
}

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 worldPos : WORLD_POS;
    float3 tangent : TANGENT;
};

float4 main(PSInput input) : SV_Target
{
    float2 distortion = 0.f;
    float opacity = 1.f;
    
    float2 texCoord = input.texCoord * textureScale;
    float2 distTexCoord = input.texCoord * textureScaleDist;
    float3 lightPos = pointLightPosition;
    //float2 texCoord = input.texCoord;
    //float2 tileOffset = float2(1.0f, 1.f) / textureScale;
    //texCoord += tileOffset;
    //texCoord *= textureScale;
    //texCoord -= tileOffset;
    //texCoord += tileOffset;
    
    if (hasDistortion)
        distortion = (distortionMap.Sample(samplerState, distTexCoord + texCoordoffsetDist).xy - 0.5f) / distDiv;

    distortion += texCoordOffset;
    
    float3 normal = input.normal;
    float3x3 tbnMatr = 0;
    if (hasNormal)
    {
        float3 mappedNormal = normalMap.Sample(samplerState, texCoord + distortion) * 2.f - 1.f;
        float3 normTan = normalize(input.tangent);
        float3 biNormal = (LH == 1) ? normalize(cross(normTan, normal)) : normalize(cross(normal, normTan));
        tbnMatr = float3x3(normTan, biNormal, normal);
        normal = normalize(float3(mul(mappedNormal, tbnMatr)));
    }
    
    float4 diffuse = diffuseMap.Sample(samplerState, texCoord + distortion);
    if (hasOpacityMap)
        opacity = opacityMap.Sample(samplerState, texCoord + distortion);
    float charDirLight = (characterLight[0].w != 0.f) ? dot(normal, normalize(characterLight[0].xyz)) * characterLight[0].w :
    0.f;
    
    
    float3 lightVector = lightPos.xyz - input.worldPos;

    float distance = length(lightVector);
    
    // Normalized
    lightVector /= distance;
    
    float attenuation = 1.f;
    
    // clamped to remove sampling artifacts
    // Uses to calculated value as index on a lookup-table stored in a texture
    //float lightindex = (false) ? saturate(dot(lightVector, normal)) * pointlightStre : 0.f;
    float lightindex = (distance <= lightDistance) ? saturate(dot(lightVector, normal)) * pointlightStre : 0.f;
    float3 camToOb = normalize(input.worldPos - camPos.xyz);
   
    float3 specular = 0.f;
    float specDot = dot(camToOb, -reflect(-lightVector, normal));
    if (specDot > 0.f)
        specular = pow(specDot, specularInstensity);
    
    float att = 1.f / dot(atten.xxx, float3(1.f, distance, distance));
    //float att = 1.f / dot(atten.xxx, float3(1.f, distance, distance));
    
    specular *= atten;
    //diffuse *= att;
    
    lightindex /= distance;
    //lightindex += specular;
    lightindex += charDirLight;
    lightindex = clamp(lightindex, 0.01f, 0.99f);
    float3 cellLightStr = ZAToon.Sample(samplerState, float2(lightindex, .5f)).xyz;
    float3 warpedSpecular = clamp(specular, 0.01f, 0.99f);
    warpedSpecular = ZAToon.Sample(samplerState, float2(warpedSpecular.z, .5f)).xyz;
    
    cellLightStr += ambientColor * ambientStr;
    
    //cellLightStr /= distance;
    

    float rimDot = 0;
    if (rim == 1)
        rimDot = 1 - dot(-camToOb, normal);
    
    float rimamount = 0.85f;
    
    float rimIntesnity = smoothstep(rimamount - 0.001, rimamount + 0.001f, rimDot);
    
    //float3 final = warpedSpecular;
    float3 final = diffuse.xyz * (cellLightStr) + (rimDot.xxx * rimColor) + specular;

    
    
    
    
    return float4(final, opacity * transparency);
}