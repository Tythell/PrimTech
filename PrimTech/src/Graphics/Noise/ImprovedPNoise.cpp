#include "ImprovedPNoise.h"

float PerlinWikipedia::value(float x, float y)
{
	int x0 = (int)floor(x);
	int x1 = x0 + 1;
	int y0 = (int)floor(y);
	int y1 = y0 + 1;

	// Determine interpolation weights
	// Could also use higher order polynomial/s-curve here
	float sx = x - (float)x0;
	float sy = y - (float)y0;

	// Interpolate between grid point gradients
	float n0, n1, ix0, ix1, value;

	n0 = dotGridGradient(x0, y0, x, y);
	n1 = dotGridGradient(x1, y0, x, y);
	ix0 = interpolate(n0, n1, sx);

	n0 = dotGridGradient(x0, y1, x, y);
	n1 = dotGridGradient(x1, y1, x, y);
	ix1 = interpolate(n0, n1, sx);

	value = interpolate(ix0, ix1, sy);
	return value;
}

float PerlinWikipedia::Perlin2D(int x, int y, float hz)
{
	float fx, fy;

	fx = (float)x * hz;
	fy = (float)y * hz;

	float v = value(fx, fy);
	if (v < m_smallestNum)
		m_smallestNum = v;

	if (v > m_biiggestNum)
		m_biiggestNum = v;

	return v;
}

float PerlinWikipedia::interpolate(float a0, float a1, float w)
{
	return (a1 - a0) * w + a0;
}

sm::Vector2 PerlinWikipedia::randomGradient(int ix, int iy)
{
	const unsigned w = 8 * sizeof(unsigned);
	const unsigned s = w / 2;
	unsigned a = ix, b = iy;
	a *= 3284157443; b ^= a << s | a >> w - s;
	b *= 1911520717; a ^= b << s | b >> w - s;
	a *= 2048419325;
	float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
	sm::Vector2 v;
	v.x = cos(random); v.y = sin(random);
	return v;
}

float PerlinWikipedia::dotGridGradient(int ix, int iy, float x, float y)
{
	sm::Vector2 gradient = randomGradient(ix, iy);

	float dx = x - (float)ix;
	float dy = y - (float)iy;

	return (dx * gradient.x + dy * gradient.y);
}
