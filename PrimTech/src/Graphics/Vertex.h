#pragma once
#include "../Math/Math.h"

//struct VertexQuad
//{
//	VertexQuad(float x, float y):
//		position(x,y)/*, uv(u, v)*/ {}
//
//	DirectX::XMFLOAT2 position;
//	//DirectX::XMFLOAT2 uv;
//};

struct Vertex3D
{
	sm::Vector3 position;
	sm::Vector2 texCoord;
	sm::Vector3 normal;
	sm::Vector3 tangent;
	sm::Vector3 bitangent;
	sm::Vector3 color = {1.f,1.f,1.f};
	bool operator==(const Vertex3D& obj)
	{
		if (position == obj.position && texCoord == obj.texCoord && normal == obj.normal)
			return true;
		return false;
	}
	void MarkDuplicate()
	{
		texCoord = { -1,-1 }; // texCoord  will normally never be -1
	}
	bool IsDuplicate() const
	{
		if (texCoord.x == -1)
			return true;
		else
			return false;
	}
};

struct BBVertex
{
	sm::Vector3 m_position;
	sm::Vector3 m_color;
};
