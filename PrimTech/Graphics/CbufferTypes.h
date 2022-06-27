#pragma once
#include "Camera.h"

namespace hlsl
{
	using float2 = sm::Vector2;
	using float3 = sm::Vector3;
	using float4 = sm::Vector4;
	using float4x4 = sm::Matrix;
	using matrix = sm::Matrix;

	using int2 = d::XMINT2;
	using int3 = d::XMINT3;
	using int4 = d::XMINT4;

	//struct bool2 { bool x; bool y; };
	//struct bool3 { bool x; bool y; bool z; };
	//struct bool4 { bool x; bool y; bool z; bool w; };

	struct cbvTransformsQuad
	{
		float4x4 world;
		float4x4 viewProj;
		float4 color;
	};

	struct cbpWorldTransforms3D
	{
		float4x4 world;
		float4x4 viewProj;
		float2 texCoordOffset = {0.f,0.f};
		float2 pad;
	};

	struct cbpLightBuffer
	{
		float3 direction;
		float p;
		float3 position;
		float ambientStr;
		float3 ambientLight;
		float attenuation;
		float3 pointLightColor;
		float pointlightStr;
	};

	struct cbpMaterialBuffer
	{
		int hasDistortion = 0;
		float2 texCoordOffset;
		float transparency = 1.f;
		float2 texCoordoffsetDist;
		int2 pad;
	};
}
