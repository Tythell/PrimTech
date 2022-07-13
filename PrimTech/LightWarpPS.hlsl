Texture2D diffuseMap : DIFFUSEMAP : register(t0);
Texture2D distortionMap : DISTORTIONMAP : register(t1);
Texture2D ZAToon : ZATOON : register(t2);
Texture2D normalMap : NORMALMAP : register(t3);
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
    int3 pad;
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
    
    float2 texCoord = input.texCoord * textureScale;
    float3 lightPos = pointLightPosition;
    //float2 texCoord = input.texCoord;
    //float2 tileOffset = float2(1.0f, 1.f) / textureScale;
    //texCoord += tileOffset;
    //texCoord *= textureScale;
    //texCoord -= tileOffset;
    //texCoord += tileOffset;
    
    if (hasDistortion)
        distortion = (distortionMap.Sample(samplerState, texCoord + texCoordoffsetDist).xy - 0.5f) / distDiv;
    
    distortion += texCoordOffset;
    
    float3 faceNormal = input.normal;
    float3x3 tbnMatr = 0;
    if(hasNormal)
    {
        float3 mappedNormal = normalMap.Sample(samplerState, texCoord + distortion) * 2.f - 1.f;
        float3 normTan = normalize(input.tangent);
        float3 biNormal = normalize(cross(faceNormal, normTan));
        tbnMatr = float3x3(normTan, biNormal, faceNormal);
        //faceNormal = biNormal;
        faceNormal = float3(mul(mappedNormal, tbnMatr));
    }
    
    float4 diffuse = diffuseMap.Sample(samplerState, texCoord + distortion);
    //float dirLight = dot(faceNormal, direction);
    
    float3 lightVector = lightPos.xyz - input.worldPos;

    float distance = length(lightVector);
    
    // Normalized
    lightVector /= distance;
    
    float attenuation = 1.f;
    
    // clamped to remove sampling artifacts
    // Uses to calculated value as index on a lookup-table stored in a texture
    //float lightindex = (false) ? saturate(dot(lightVector, faceNormal)) * pointlightStre : 0.f;
    float lightindex = (distance <= lightDistance) ? saturate(dot(lightVector, faceNormal)) * pointlightStre : 0.f;
    float3 camToOb = normalize(input.worldPos - camPos.xyz);
    //float spec = 0.f;
    //if (lightindex < 0.f)
    //{
    //    float3 v = reflect(-lightVector, faceNormal);
    //    float specFactor = pow(max(dot(v, -camToOb), 0.f), specularInstensity);

    //    spec = specFactor * specularInstensity;
    //}
    //float3 spec = pow(max(dot(camToOb, -reflect(camToOb, faceNormal)), 0), atten);
    float3 specular = pow(max(dot(camToOb, -reflect(-lightVector, faceNormal)), 0.f),specularInstensity);
    
    float att = 1.f / dot(atten.xxx, float3(1.f, distance, distance));
    
    specular *= att;
    diffuse *= att;
    
    lightindex += specular;
    lightindex = clamp(lightindex, 0.01f, 0.99f);
    float3 cellLightStr = ZAToon.Sample(samplerState, float2(lightindex, .5f)).x;
    
    //cellLightStr /= distance;
    

    float rimDot = 0;
    if (rim == 1)
        rimDot = 1 - dot(-camToOb, faceNormal);
    
    float3 final = diffuse.xyz * (cellLightStr) + (rimDot.rrr * rimColor);

    
    
    
    
    return float4(faceNormal, transparency);
}