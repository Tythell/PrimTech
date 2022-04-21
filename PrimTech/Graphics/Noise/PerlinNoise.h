#pragma once
#include<vector>
#include <numeric>
#include "../Camera.h"

//float InterLerp(float a0, float a1, float w)
//{
//	return (a1 - a0) * w + a0;
//}

class Perlin
{
public:
	Perlin();
	Perlin(unsigned int seed, uint16_t resolution, uint16_t dimensions = 2);
	//void Recast(unsigned int seed);
	static float perlinn(float x, float y);
private:
	static sm::Vector2 RandomGradient(int ix, int iy);
	static float dotGridGradient(int ix, int iy, float x, float y);

	float grids[256];
	uint16_t m_seed;
	uint16_t m_resolution;
	uint16_t m_dimensions;
};