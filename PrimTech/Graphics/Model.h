#pragma once
#include "Transform.h"
#include"Buffer.h"
#include"Vertex.h"
#include <string>
#include <d3d11.h>
#include <fstream>

class Model
{
public:
	Model();
	bool LoadObj(const std::string path, ID3D11Device*& pDevice, ID3D11DeviceContext*& pDc);
	void LoadTriangle(ID3D11Device*& pDevice, ID3D11DeviceContext*& pDc);
	void Draw();
private:
	Buffer<Vertex3D> m_vbuffer;
	ID3D11DeviceContext* dc = nullptr;
	Transform m_transform;
};