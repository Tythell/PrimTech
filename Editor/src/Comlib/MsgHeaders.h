#pragma once
#include "Math/Math.h"


enum class Headers
{
	MESSAGE = 0,
	LonePixel,
	PixelArrayStart
};

struct SectionHeader
{
	Headers header = Headers::MESSAGE;
	size_t msgLen = 0;
	size_t msgID = 0;
};

using PixelArrayStart = size_t;

struct PixelChange
{
	uint2 pixel = uint2(0.f);
	float4 color = float4(0.f);
};

struct WholeTextureChange
{
	uint2 textureDimesnions = uint2(0);
	float4* colors = nullptr;
};