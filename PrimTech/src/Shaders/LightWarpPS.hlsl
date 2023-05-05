//#pragma pack_matrix(row_major)

Texture2D ZAToon : ZATOON : register(t0);
Texture2D diffuseMap : DIFFUSEMAP : register(t1);
Texture2D distortionMap : DISTORTIONMAP : register(t2);
Texture2D normalMap : NORMALMAP : register(t3);
Texture2D opacityMap : NORMALMAP : register(t4);

Texture2D shadowMap : SHADOWMAP : register(t10);


struct Light
{
    float4 pos;
    float4 clr;
    float4 dire;
};

StructuredBuffer<Light> mySb : LIGHTS : register(t11);

SamplerState wrapSampler : SAMPLER : register(s0);
SamplerState clampSampler : CLAMPSAMPLER : register(s1);
SamplerState shadowSampler : SHADOWSAMPLER : register(s2);

cbuffer LightBuffer : register(b0)
{
    float3 camPos;
    uint numLights;
    float3 shadowDir;
    float cbShadowBias;
};

// Material flags
#define MaterialFlag_eHasDiffuse        (1 << 0)
#define MaterialFlag_eHasDistortion	    (1 << 1)
#define MaterialFlag_eHasNormal         (1 << 2)
#define MaterialFlag_eHasOpacity		(1 << 3)
cbuffer MaterialBuffer : register(b1)
{
    float3 diffuseColor = { 1.f, 1.f, 1.f };
    uint flags;

    float2 texCoordOffset;
    float transparency = 1.f;
    int distDiv;

    float2 texCoordoffsetDist;
    float textureScale;
    int pad;

    float3 rimColor;
    float textureScaleDist;
}

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 worldPos : WORLD_POS;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 clipSpace : CLIPSPACE;
    float3 vcolor : COLOR;
};

float3 calcNormal(in float2 texCoord, in float3 normal, in float3 tan, in float3 biTan)
{
    if (flags & MaterialFlag_eHasNormal)
    {
        float3 mappedNormal = normalMap.Sample(wrapSampler, texCoord).xyz * 2.f - 1.f;
 
        float3x3 tbnMatr = float3x3(normalize(tan), normalize(biTan), normalize(normal));
    
        return normalize(float3(mul(mappedNormal, tbnMatr)));
    }
    else
        return normal;
}

float calcShadow(in float4 clipspace, in float3 normal)
{
    float2 ndc = ((clipspace.xy) * .5f + .5f);
    float2 projTexCoord = float2(ndc.x, 1.f - ndc.y);
    
    float bias = cbShadowBias;
    float shadowBias = max(.04f * (1.f - dot(normalize(shadowDir), normal)), .005f);
    
    // pcf
    float shadow = 0.f;
    uint shadowmapsize;
    shadowMap.GetDimensions(shadowmapsize, shadowmapsize);
    float2 texelSize = 1.f / shadowmapsize;
    
    for (int x = -1; x <= 1; x++)
        for (int y = -1; y <= 1; y++)
        {
            float depth = shadowMap.Sample(shadowSampler, projTexCoord + int2(x, y) * texelSize).r;
            shadow += (depth > clipspace.z - shadowBias ? 1.0f : 0.0f);
        }
    
    return shadow / 9.f;
}

float4 main(PSInput input) : SV_Target
{
    float2 distortion = 0.f;
    float opacity = 1.f;
    
    float2 texCoord = input.texCoord * textureScale;
    float2 distTexCoord = input.texCoord * textureScaleDist;
    float3 lightPos = float3(0.f,0.f,0.f); // TODO: REMOVE LATER
    
    if (flags & MaterialFlag_eHasDistortion)
        distortion = (distortionMap.Sample(wrapSampler, distTexCoord + texCoordoffsetDist).xy - 0.5f) / distDiv;

    distortion += texCoordOffset;
    
    
    float3x3 tbnMatr = 0;
    float3 normal = calcNormal(texCoord + distortion, input.normal, input.tangent, input.bitangent);
    float shadow = calcShadow(input.clipSpace, normal);
    
    float4 diffuse;
    if (flags & MaterialFlag_eHasDiffuse)
        diffuse = saturate(diffuseMap.Sample(wrapSampler, texCoord + distortion) /** float4(input.vcolor, 1.f)*/);
    else
        diffuse = float4(diffuseColor, 1.f);
    
    diffuse.w = 1.f;
    //float4 diffuse = diffuseMap.Sample(samplerState, texCoord + distortion);
    
    if (flags & MaterialFlag_eHasOpacity)
        opacity = opacityMap.Sample(wrapSampler, texCoord + distortion).x;
    else
        opacity = diffuse.w;
    
    float attenuation = 1.f;
    
    float3 lightVector = lightPos.xyz - input.worldPos;
    float distance = length(lightVector);
    //attenuation = max(0, 1.f - (distance / lightDistance.x));
    
    //// spotlight
    //float3 spotLightVector = spotLightPos - input.worldPos;
    //float sldistance = length(spotLightVector);
    //spotLightVector /= sldistance;
    
    //float3 L2 = spotLightAngle;
    //float rho = dot(-spotLightVector, L2);
    //attenuation *= saturate((rho - spotLightAngle.y) / (spotLightAngle.x - spotLightAngle.y));
    
    // Normalized
    lightVector /= distance;
    
    
    
    // Uses to calculated value as index on a lookup-table stored in a texture
    //float lightindex = (false) ? saturate(dot(lightVector, normal)) * pointlightStre : 0.f;
   
    
    float3 lightValue = 0.f;
    float3 amb = float3(0.f,0.f,0.f);
    
    uint numOfLights = numLights;
    for (int i = 0; i < numOfLights; i++)
    {
        Light light = mySb[i];
        if (light.pos.w == 1.f)
        {
            float3 lightVec = light.pos.xyz - input.worldPos;
            float d = length(lightVec);
            //attenuation = max(0, 1.f - (d / lightDistance.x));
            lightVec /= d;
        
            float3 ligVal = saturate(dot(lightVec, normal)) * light.clr.rgb * light.clr.w;
        
            lightValue += (ligVal / d) * shadow;
        }
        else if (light.dire.w == 1)
        {
            float3 direct = normalize(mySb[i].dire.xyz);
            float3 lightVecc = (max(0, -dot(normal, direct)) * light.clr.w) * shadow;
            lightValue += lightVecc;
        }
        else
        {
            amb = light.clr.xyz * light.clr.w;
        }
        
    }
    
    lightValue += amb;
    
    //float lightindex = (distance <= lightDistance) ? saturate(dot(lightVector, normal)) : 0.f;
    //float3 camToOb = normalize(input.worldPos - camPos.xyz);
   
    //float3 specular = 0.f;
    //float specDot = dot(camToOb, -reflect(-lightVector, normal));
    //if (specDot > 0.f)
    //    specular = pow(specDot, specularInstensity);
    
    //float att = 1.f / dot(atten.xxx, float3(1.f, distance, distance));
    //float att = 1.f / dot(atten.xxx, float3(1.f, distance, distance));
    
    //specular *= atten;
    //diffuse *= att;
    
    //lightindex /= distance;
    //lightindex += specular;
    //float3 cellLightStr = ZAToon.Sample(clampSampler, float2(lightindex, .5f)).xyz;
    //specular = ZAToon.Sample(clampSampler, float2(specular.z, .5f)).xyz;
    

    //float rimDot = 0;
    //if (rim == 1)
    //    rimDot = 1 - dot(-camToOb, normal);
    
    //float rimamount = 0.85f;
    
    //float rimIntesnity = smoothstep(rimamount - 0.06, rimamount + 0.06f, rimDot);
    
    //float3 final = warpedSpecular;
    float3 final = diffuse.xyz * lightValue;
    //float3 final = diffuse.xyz * (cellLightStr) + (rimDot.xxx * rimColor) + specular;

    //return float4(normal, 1.f);
    return float4(final, opacity * transparency);
}