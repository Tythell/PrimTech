#pragma once
#include<vector>
#include <numeric>
#include "../Camera.h"

//float InterLerp(float a0, float a1, float w)
//{
//	return (a1 - a0) * w + a0;
//}

#define WIDTH
#define HEIGHT 200
#define WRAP_SIZE 256

class PerlinNoise2D
{
public:
	void Init(int seed = 0);
	float Value(float x, float y);
	~PerlinNoise2D();

private:
	sm::Vector2* m_gradients;

};

class OLCPerlinLikeNoise
{
public:
	OLCPerlinLikeNoise();
	void PerlinNoise1D(int count, float* seed, int nrofOctaves, float* fOutput);
	void PerlinNoise2D(int resolution, float* seed, int nrofOctaves, float* output);

private:
	float* m_noiseSeed1D = nullptr;
	float* m_perlinNoise1D = nullptr;
	int m_outputSize = 256;
	int m_octaveCount = 0;

	// 2D
	int m_outputWidth = 256, m_outputHeight = 256;
	float* m_noiseSeed2D = nullptr;
	float* m_perlinNoise2D = nullptr;

	int m_mode = 1;
};