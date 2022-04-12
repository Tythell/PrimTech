#pragma once
#include "../Math/SimpleMath.h"
#include <array>

struct Vertex
{
	Vertex(float x, float y, float z, float r, float g, float b):
		position(x,y, 0.f), color(r,g,b) {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 color;
};

