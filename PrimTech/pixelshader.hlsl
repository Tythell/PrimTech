Texture2D diffuseMap : DIFFUSEMAP : register(t0);
Texture2D distortionMap : DISTORTIONMAP : register(t1);
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
}

float4 main(PSInput input) : SV_Target
{
    float distortion;
    if(hasDistortion)
        distortion = distortionMap.Sample(samplerState, input.texCoord).x - 0.5f /*/ 10*/;
    else
        distortion = 0.f;
    
    distortion += texCoordOffset;
    float4 diffuse = diffuseMap.Sample(samplerState, input.texCoord + distortion);
    float attenuation = 1.f;
    float3 faceNormal = input.normal;
    
    
    
    //float dirLight = dot(faceNormal, direction);
    
    float3 obToLight = position.xyz - input.worldPos;

    float distance = length(obToLight);
    
    float pointLightStr = max(0, dot(normalize(obToLight), faceNormal));
    pointLightStr /= distance;
    
    //pointLightStr += ambientStr * ambientLight;
    
    float3 ambient = ambientLight * ambientStr;
    
    float3 light = ambient + (pointLightStr * (pointLightColor * pointlightStre));
    
    float3 final = diffuse.xyz * light;
    
    return float4(final, transparency);
}