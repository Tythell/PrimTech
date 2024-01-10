#pragma once
#include "Math/Math.h"

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

		//struct bool2 { bool x; bool y; };
		//struct bool3 { bool x; bool y; bool z; };
		//struct bool4 { bool x; bool y; bool z; bool w; };

		struct cbvTransformsQuad
		{
			float4x4 world = float4x4(0.f);
			float4x4 viewProj = float4x4(0.f);
			float4 color = float4(0.f);
		};

		struct cbpWorldTransforms3D
		{
			float4x4 world = float4x4(0.f);
			float4x4 viewProj = float4x4(0.f);
			float4x4 lightViewProj = float4x4(0.f);
		};

		struct Light
		{
			float4 pos = float4(0.f);
			float4 clr = float4(0.f);
			float4 dire = float4(0.f);
		};

		struct cbpLightBuffer
		{
			float3 camPos = float3(0);
			uint numLights = 0u;
			float3 shadowDir = float3(0);
			float cbShadowBias = 0.f;
		};

		struct cbpMaterialBuffer
		{
			float3 diffuseColor = { 1.f,1.f,1.f };
			uint flags = 0;

			float2 texCoordOffset = float2(0.f);
			float transparency = 1.f;
			uint distDiv = 0;

			float2 texCoordoffsetDist = float2(0.f);
			float textureScale = 0.f;
			int pad = 0;

			float3 rimColor = float3(0.f);
			float textureScaleDist = 0.f;
		};
	}
}