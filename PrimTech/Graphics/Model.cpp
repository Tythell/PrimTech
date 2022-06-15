#include "Buffer.h"
#include"../Math/Math.h"
#include<vector>
#include "Model.h"
#include<iostream>
#include <fstream>

Model::Model()
{
}

void Model::Init(const std::string path, ID3D11Device*& pDevice, ID3D11DeviceContext*& pDc, Buffer<hlsl::cbWorldTransforms3D>& pCbuffer)
{
	dc = pDc;
	mp_cbTransformBuffer = &pCbuffer;
	LoadObj(path, pDevice);
}

bool Model::LoadObj(const std::string path, ID3D11Device*& pDevice)
{
	std::string s;
	std::vector<sm::Vector4> v;
	std::vector<sm::Vector3> vn;
	std::vector<sm::Vector2> vt;
	std::vector<UINT> posIndex;
	std::vector<UINT> normalIndex;
	std::vector<UINT> texcoordsIndex;

	std::vector<Vertex3D> shape;

	std::ifstream reader(path);

	if (!reader.is_open())
	{
		Popup::Error(path + " not found");
		return false;
	}
	while (std::getline(reader, s))
	{
		std::string input;
		std::cout << s;
		reader >> input;
		if (input == "v")
		{
			DirectX::XMFLOAT4 vertex;
			reader >> vertex.x >> vertex.y >> vertex.z;
			v.emplace_back(vertex);
		}
		else if (input == "vt")
		{
			DirectX::XMFLOAT2 uv;
			reader >> uv.x >> uv.y;
			vt.emplace_back(uv);
		}
		else if (input == "vn")
		{
			DirectX::XMFLOAT3 normal;
			reader >> normal.x >> normal.y >> normal.z;
			vn.emplace_back(normal);
		}
		else if (input == "f")
		{
			int vIndexTemp = -1;
			int vtIndexTemp = -1;
			int vnIndexTemp = -1;
			char slash;
			for (int i = 0; i < 3; i++)
			{
				reader >> vIndexTemp >> slash >> vtIndexTemp >> slash >> vnIndexTemp;
				vIndexTemp--;
				vnIndexTemp--;
				vtIndexTemp--;
				Vertex3D tempVert;
				tempVert.position = v[vIndexTemp];
				tempVert.normal = vn[vnIndexTemp];
				tempVert.texCoord = vt[vtIndexTemp];
				shape.emplace_back(tempVert);
			}
		}
	}
	if (FAILED(m_vbuffer.CreateVertexBuffer(pDevice, shape.data(), (unsigned int)shape.size())))
	{
		Popup::Error("Failed to create vertex buffer");
		return false;
	}
	return true;
}

void Model::Draw()
{
	UINT offset = 0;
	dc->VSSetConstantBuffers(0, 1, mp_cbTransformBuffer->GetReference());
	mp_cbTransformBuffer->Data().world = GetWorldTransposed();
	mp_cbTransformBuffer->UpdateCB();
	dc->IASetVertexBuffers(0, 1, m_vbuffer.GetReference(), m_vbuffer.GetStrideP(), &offset);
	dc->Draw(m_vbuffer.GetBufferSize(), 0);
}
