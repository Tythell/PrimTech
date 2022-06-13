#pragma once
#include "../Math/Math.h"

struct VertexQuad
{
	VertexQuad(float x, float y):
		position(x,y)/*, uv(u, v)*/ {}

	DirectX::XMFLOAT2 position;
	//DirectX::XMFLOAT2 uv;
};

struct Vertex3D
{
	sm::Vector3 position;
	sm::Vector3 normal;
	sm::Vector2 texCoord;
};

