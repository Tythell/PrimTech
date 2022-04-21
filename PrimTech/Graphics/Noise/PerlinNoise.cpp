#include "PerlinNoise.h"
#include "../Camera.h"

//Perlin::Perlin()
//{}

//Perlin::Perlin(unsigned int seed, uint16_t resolution, uint16_t dimensions):
	//m_seed(seed), m_resolution(resolution), m_dimensions(dimensions)
//{
	//srand(seed);
	//Recast(seed);
//}

//sm::Vector2 Perlin::RandomGradient(int ix, int iy)
//{
	//const unsigned w = 8 * sizeof(unsigned);
	//const unsigned s = w / 2;
	//unsigned a = ix, b = iy;

	//int f = 10 >> 1;

	//a *= 3284157443; b ^= a << s | a >> w - s;
	//b *= 1911520717; a ^= b << s | b >> w - s;
	//a *= 2048419325;
	//float random = a * (d::XM_PI / ~(0u >> 1));
	//sm::Vector2 v;
	//v.x = cos(random); v.y = sin(random);
	//return v;
	//return sm::Vector2();
//}

//float Perlin::dotGridGradient(int ix, int iy, float x, float y)
//{
	//sm::Vector2 gradient = RandomGradient(ix, iy);
	//float dx = x - (float)ix;
	//float dy = y - (float)iy;

	//return (dx*gradient.x + dy*gradient.y);
	//return 0.f;
//}

//float Perlin::perlinn(float x, float y)
//{
	//int x0 = (int)floor(x);
	//int x1 = x0 + 1;
	//int y0 = (int)floor(y);
	//int y1 = y0 + 1;

	//float sx = x - (float)x0;
	//float sy = y - (float)y0;

	//float n0, n1, ix0, ix1, value;

	//n0 = dotGridGradient(x0, y0, x, y);
	//n1 = dotGridGradient(x1, y0, x, y);

	//n0 = dotGridGradient(x0, y0, x, y);
	//n1 = dotGridGradient(x1, y0, x, y);
	//ix0 = InterLerp(n0, n1, sx);

	//n0 = dotGridGradient(x0, y1, x, y);
	//n1 = dotGridGradient(x1, y1, x, y);
	//ix1 = InterLerp(n0, n1, sx);

	//value = InterLerp(ix0, ix1, sy);

	//return value;
	//return 0.f;
//}

void PerlinNoise::Noise1D(int count, float* fSeed, int nOctaves, float* fOutput)
{
	for (int x = 0; x < count; x++)
	{

	}
}
