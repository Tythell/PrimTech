#pragma once
#include"../../Math/Math.h"
class PerlinWikipedia
{
public:
	float Perlin2D(int x, int y, float hz);
private:
	float value(float x, float y);
	float interpolate(float a0, float a1, float w);
	sm::Vector2 randomGradient(int ix, int iy);
	float dotGridGradient(int ix, int iy, float x, float y);
	float m_smallestNum = 0.f;
	float m_biiggestNum = 0.f;
};