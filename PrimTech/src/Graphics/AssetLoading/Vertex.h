#pragma once
#include "../Math/Math.h"

namespace PrimtTech
{
	struct Vertex3D
	{
		sm::Vector3 position;
		sm::Vector2 texCoord;
		sm::Vector3 normal;
		sm::Vector3 tangent;
		sm::Vector3 bitangent;
		//sm::Vector3 color = { 1.f,1.f,1.f };
	};

	struct BBVertex
	{
		BBVertex(){}
		BBVertex(float x, float y, float z)
		{
			m_position.x = x;
			m_position.y = y;
			m_position.z = z;
		}
		sm::Vector3 m_position = { 0.f,0.f,0.f };
		sm::Vector3 m_color = { 1.f,1.f,1.f };
	};

}

//struct VertexQuad
//{
//	VertexQuad(float x, float y):
//		position(x,y)/*, uv(u, v)*/ {}
//
//	DirectX::XMFLOAT2 position;
//	//DirectX::XMFLOAT2 uv;
//};

