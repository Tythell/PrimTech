#pragma once
#include<vector>
#include <numeric>
#include "ImprovedPNoise.h"

class Perlin
{
public:
	Perlin(int rectangles = 4);
	~Perlin();

private:
	d::XMINT2* m_grid = nullptr;
	float* m_pixelGrid = nullptr;
	const int NRECTS;
};

#define WIDTH
#define HEIGHT 200
#define WRAP_SIZE 256

class PerlinNoise2DIndian
{
public:
	void Init(int seed = 0);
	float Value(float x, float y);
	~PerlinNoise2DIndian();

private:
	sm::Vector2* m_gradients;
	

};
//
//class OLCPerlinLikeNoise
//{
//public:
//	OLCPerlinLikeNoise();
//	void PerlinNoise1D(int count, float* seed, int nrofOctaves, float* fOutput);
//	void PerlinNoise2D(int resolution, float* seed, int nrofOctaves, float* output);
//
//private:
//	float* m_noiseSeed1D = nullptr;
//	float* m_perlinNoise1D = nullptr;
//	int m_outputSize = 256;
//	int m_octaveCount = 0;
//
//	// 2D
//	int m_outputWidth = 256, m_outputHeight = 256;
//	float* m_noiseSeed2D = nullptr;
//	float* m_perlinNoise2D = nullptr;
//
//	int m_mode = 1;
//};