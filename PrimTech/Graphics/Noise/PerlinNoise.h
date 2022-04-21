#pragma once
#include<vector>
#include <numeric>
#include "../Camera.h"

//float InterLerp(float a0, float a1, float w)
//{
//	return (a1 - a0) * w + a0;
//}

class PerlinNoise
{
public:
	void Noise1D(int count, float* fSeed, int nOctaves, float* fOutput);

private:

};