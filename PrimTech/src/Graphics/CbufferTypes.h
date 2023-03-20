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

		struct Light
		{
			float4 pos;
			float4 clr;
			float4 dire;
		};

		struct cbpLightBuffer
		{
			float3 direction;
			float specularInstensity;
			float3 pointLightPosition;
			//float ambientStr;
			//float3 ambientColor;
			float atten;
			float3 pointLightColor;
			float pointlightStre;
			float3 camPos;
			float lightDistance;
			float3 shadowDir;
			float cbShadowBias;
			float3 spotLightPos;
			uint numLights;
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
			int pad;

			float3 rimColor;
			float textureScaleDist;
		};
	}
}