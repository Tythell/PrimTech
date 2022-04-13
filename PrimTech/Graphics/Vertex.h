#pragma once
#include "../Math/SimpleMath.h"
#include <array>

struct Vertex
{
	Vertex(float x, float y, float z, float u, float v):
		position(x,y, z), uv(u,v) {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 uv;
};

