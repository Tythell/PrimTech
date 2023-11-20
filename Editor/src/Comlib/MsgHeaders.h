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
	uint2 pixel;
	float4 color;
};

struct WholeTextureChange
{
	uint2 textureDimesnions;
	float4* colors = nullptr;
};