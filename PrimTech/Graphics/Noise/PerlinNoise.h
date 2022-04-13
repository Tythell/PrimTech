#pragma once
#include<vector>
#include <numeric>
#include "../Camera.h"

float Interpolate(float a0, float a1, float w)
{
	return (a1 - a0) * w + a0;
}

class Perlin
{
public:
	Perlin();
	Perlin(unsigned int seed, uint16_t resolution, uint16_t dimensions = 2);
	void Recast(unsigned int seed);
private:
	sm::Vector2 RandomGradient(int ix, int iy);
	float dotGridGradient(int ix, int iy, float x, float y);
	float perlinn(float x, float y);

	float grids[256];
	uint16_t m_seed;
	uint16_t m_resolution;
	uint16_t m_dimensions;
};