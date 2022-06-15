Texture2D diffuseMap : DIFFUSEMAP : register(t0);
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

float4 main(PSInput input) : SV_Target
{
    float3 diffuse = diffuseMap.Sample(samplerState, input.texCoord);
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
    
    float3 final = diffuse * light;
    
    return float4(final, 1.f);
}