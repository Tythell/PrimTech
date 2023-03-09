#pragma once
#include "Camera.h"

namespace PrimtTech {
	namespace hlsl {
		enum eMaterialFlag
		{
			MaterialFlag_eNULL,
			MaterialFlag_eHasDiffuse = (1 << 0),
			MaterialFlag_eHasDistortion = (1 << 1),
			MaterialFlag_eNormal = (1 << 2),
			MaterialFlag_eOpacity = (1 << 3),
		};

		using float2 = sm::Vector2;
		using float3 = sm::Vector3;
		using float4 = sm::Vector4;
		using float4x4 = sm::Matrix;
		using matrix = sm::Matrix;

		using int2 = d::XMINT2;
		using int3 = d::XMINT3;
		using int4 = d::XMINT4;

		using uint = UINT;
		using uint2 = d::XMUINT2;
		using uint3 = d::XMUINT3;
		using uint4 = d::XMUINT4;

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
			float4x4 lightViewProj;
		};

		struct cbpLightBuffer
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
			float pointLightDistance;
			float3 shadowDir;
			float cbShadowBias;
			float3 spotLightPos;
			float pad1;
			float3 spotLightAngle;
			float pad2;
		};

		struct cbpMaterialBuffer
		{
			float3 diffuseColor = { 1.f,1.f,1.f };
			uint flags;

			float2 texCoordOffset;
			float transparency = 1.f;
			int distDiv;

			float2 texCoordoffsetDist;
			float textureScale;
			int rim; // bool

			float3 rimColor;
			float textureScaleDist;
		};
	}
}