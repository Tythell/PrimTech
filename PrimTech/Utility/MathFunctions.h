#pragma once

namespace pm
{
	inline float Lerp(float a, float b, float t)
	{
		return a * (1.f - t) + b * t;
	}
}

