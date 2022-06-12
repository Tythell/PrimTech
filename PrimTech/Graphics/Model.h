#pragma once
#include "../Math/Math.h"
#include <string>
#include <d3d11.h>
struct Vertex
{
	sm::Vector3 pos;
	sm::Vector3 normals;
	sm::Vector2 texCoord;
};

class Model
{
public:
	Model();
	void Load(const std::string path, ID3D11Device*& pDevice, ID3D11DeviceContext*& pDc);
private:
	Buffer<Vertex> m_vbuffer;

};