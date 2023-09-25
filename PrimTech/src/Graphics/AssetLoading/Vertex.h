#pragma once
#include "../Math/Math.h"

namespace PrimtTech
{
	struct Vertex3D
	{
		float3 position;
		float2 texCoord;
		float3 normal;
		float3 tangent;
		float3 bitangent;
		//float3 color = { 1.f,1.f,1.f };
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
		float3 m_position = { 0.f,0.f,0.f };
		float3 m_color = { 1.f,1.f,1.f };
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

