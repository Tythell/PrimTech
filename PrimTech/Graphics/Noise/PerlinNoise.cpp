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

//OLCPerlinLikeNoise::OLCPerlinLikeNoise()
//{
//	if (m_mode == 1)
//	{
//		m_noiseSeed1D = new float[m_outputSize];
//		m_perlinNoise1D = new float[m_outputSize];
//		for (int i = 0; i < m_outputSize; i++)
//		{
//			m_noiseSeed1D[i] = (float)rand() / RAND_MAX;
//		}
//		PerlinNoise1D(m_outputSize, m_noiseSeed1D, m_octaveCount, m_perlinNoise1D);
//	}
//	else if (m_mode == 2)
//	{
//		const int wxh = m_outputWidth * m_outputHeight;
//
//		m_noiseSeed2D = new float[wxh];
//		m_perlinNoise2D = new float[wxh];
//		for (int i = 0; i < wxh; i++)
//			m_noiseSeed2D[i] = (float)rand() / (float)RAND_MAX;
//	}
//
//
//
//
//
//}
//
//void OLCPerlinLikeNoise::PerlinNoise1D(int count, float* seed, int nrofOctaves, float* output)
//{
//	for (int x = 0; x < count; x++)
//	{
//		float noise = 0.f;
//		float scale = 1.f;
//		float scaleAcc = 0.f;
//		for (int o = 0; o < nrofOctaves; o++)
//		{
//			int pitch = count >> o;
//			int sample1 = (x / pitch) * pitch;
//			int sample2 = (sample1 + pitch) % count;
//
//			float blend = (float)(x - sample1) / (float)pitch;
//			float sample = (1.f - blend) * seed[sample1] + blend * seed[sample2];
//			noise += sample * scale;
//			scaleAcc += scale;
//			scale /= 2.f;
//		}
//		output[x] = noise / scaleAcc;
//	}
//}
//
//void OLCPerlinLikeNoise::PerlinNoise2D(int resolution, float* seed, int nrofOctaves, float* output)
//{
//	for (int x = 0; x < resolution; x++)
//		for (int y = 0; y < resolution; y++)
//		{
//			float noise = 0.f;
//			float scale = 1.f;
//			float scaleAcc = 0.f;
//			for (int o = 0; o < nrofOctaves; o++)
//			{
//				int pitch = resolution >> o;
//				int sampleX1 = (x / pitch) * pitch;
//				int sampleX2 = (y / pitch) * pitch;
//
//				int sampleY1 = (sampleX1 + pitch) % resolution;
//				int sampleY2 = (sampleY1 + pitch) % resolution;
//
//				float blendX = (float)(x - sampleX1) / (float)pitch;
//				float blendY = (float)(y - sampleY1) / (float)pitch;
//				float sampleT = (1.f - blendX) * seed[sampleY1 * resolution + sampleX1] + blendX * seed[sampleY1 * resolution + sampleX2];
//				float sampleB = (1.f - blendY) * seed[sampleY2 * resolution + sampleX1] + blendX * seed[sampleY2 * resolution + sampleX2];
//				noise += (blendY * (sampleB - sampleT) + sampleT) * scale;
//				scaleAcc += scale;
//				scale /= 2.f;
//			}
//			output[y * resolution * x] = noise / scaleAcc;
//		}
//}
//
void PerlinNoise2DIndian::Init(int seed)
{
	//srand(seed);
	const int fullWrap = pow(WRAP_SIZE, 2);
	sm::Vector2 grad[4] = {
		{1.f, 0.f},
		{0,1},
		{-1,0},
		{0,-1}
	};
	m_gradients = new sm::Vector2[fullWrap];

	for (int i = 0; i < fullWrap; i++)
	{
		m_gradients[i] = grad[rand() % 4];
	}
}
//
float PerlinNoise2DIndian::Value(float x, float y)
{
	int ix = (int)x;
	int iy = (int)y;

	float fx = x - ix;
	float fy = y - iy;

	ix = ix % WRAP_SIZE;
	iy = iy % WRAP_SIZE;

	sm::Vector2 pTL = m_gradients[(int)y * WRAP_SIZE + (int)x];
	sm::Vector2 pTR = m_gradients[(int)y * WRAP_SIZE + (int)x + 1];
	sm::Vector2 pBL = m_gradients[(int)(y + 1) * WRAP_SIZE + (int)x];
	sm::Vector2 pBR = m_gradients[(int)(y + 1) * WRAP_SIZE + ((int)x - 1)];

	sm::Vector2 dTL = { fx, fy };
	sm::Vector2 dTR = { fx - 1, fy };
	sm::Vector2 dBL = { fx, fy - 1 };
	sm::Vector2 dBR = { fx - 1, fy - 1 };

	float fTL = pTL.Dot(dTL);
	float fTR = pTR.Dot(dTR);
	float fBL = pBL.Dot(dBR);
	float fBR = pBR.Dot(dBR);

	auto lerp = [](float& a, float& b, float t)
	{
		return t * (b - a) + a;
	};

	auto smooth = [](float t)
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	};

	fx = smooth(fx);
	fy = smooth(fy);

	float fL = lerp(fTL, fBL, fy);
	float fR = lerp(fTR, fBR, fy);

	float val = lerp(fL, fR, fx);

	return (val + 1.f) / 2.f;
}

PerlinNoise2DIndian::~PerlinNoise2DIndian()
{
	delete m_gradients;
}

Perlin::Perlin(int rectangles) :
	NRECTS(rectangles)
{
	int gradGridSize = (int)pow(NRECTS + 1, 2);

	m_pixelGrid = new float[256 * 256];
	m_grid = new d::XMINT2[gradGridSize];

	d::XMINT2 grad[4] =
	{
		{1, 1},
		{-1,1},
		{1,-1},
		{-1,-1}
	};

	for (int i = 0; i < gradGridSize; i++)
	{
		m_grid[i] = grad[rand() % 4];
	}

	for (int x = 0; x < gradGridSize; x++)
		for (int y = 0; y < gradGridSize; y++)
		{

		}
}

Perlin::~Perlin()
{
	delete m_grid;
	delete m_pixelGrid;
}
