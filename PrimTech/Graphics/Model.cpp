#include "Buffer.h"
#include"../Math/Math.h"
#include<vector>
#include "Model.h"
#include<iostream>
#include <fstream>
#include "ResourceHandler.h"

Model::Model()
{
}

//void Model::SetDCandBuffer(ID3D11DeviceContext*& pdc, Buffer<hlsl::cbpWorldTransforms3D>& pCbuffer)
//{
//	dc = pdc;
//	mp_cbTransformBuffer = &pCbuffer;
//}

//ID3D11DeviceContext* Model::dc;
//Buffer<hlsl::cbpWorldTransforms3D>* Model::mp_cbTransformBuffer;

void Model::Init(const std::string path, ID3D11Device*& pD, ID3D11DeviceContext*& pDc, Buffer<hlsl::cbpWorldTransforms3D>& buffer, bool makeLeftHanded)
{
	std::string fullpath = "Assets/models/" + path;
	dc = pDc;
	mp_cbTransformBuffer = &buffer;
	int meshIndex = ResourceHandler::CheckMeshNameExists(StringHelper::GetName(fullpath));
	if (meshIndex != -1)
		mp_mesh = ResourceHandler::GetMeshAdress(meshIndex);
	else
		mp_mesh = ResourceHandler::AddMesh(fullpath);

	dc->VSSetConstantBuffers(0, 1, mp_cbTransformBuffer->GetReference());
}

void Model::Draw()
{
	UINT offset = 0;
	
	m_material.Set(dc);
	mp_cbTransformBuffer->Data().world = GetWorldTransposed();
	mp_cbTransformBuffer->UpdateCB();
	dc->IASetVertexBuffers(0, 1, mp_mesh->GetVBuffer().GetReference(), mp_mesh->GetVBuffer().GetStrideP(), &offset);
	
	
	dc->Draw(mp_mesh->GetVBuffer().GetBufferSize(), 0);
}

void Model::UpdateTextureScroll(const float& deltatime)
{
	m_material.UpdateTextureScroll(deltatime);
}

void Model::LoadTexture(std::string path, TextureType type)
{
	m_material.LoadTexture(path, type);
}

void Model::setDiffuseScrollSpeed(float x, float y)
{
	m_material.SetDiffuseScrollSpeed(x, y);
}

void Model::SetMaterialBuffer(Buffer<hlsl::cbpMaterialBuffer>& cbMaterialBuffer)
{
	m_material.SetPointers(&cbMaterialBuffer);
}

//void Model::CreateMaterial(ID3D11Device*& device, ID3D11DeviceContext*& dc)
//{
	//m_material.Create(device, dc);
//}

bool LoadObjToBuffer(std::string path, ID3D11Device*& pDevice, Buffer<Vertex3D>& vbuffer, bool makeLeftHanded)
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
			if(makeLeftHanded) vertex.z *= -1;
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
			if(makeLeftHanded) normal.z *= -1;
			vn.emplace_back(normal);
		}
		else if (input == "f")
		{
			int vIndexTemp = -1;
			int vtIndexTemp = -1;
			int vnIndexTemp = -1;
			char slash;
			Vertex3D triangle[3];
			for (int i = 0; i < 3; i++)
			{
				reader >> vIndexTemp >> slash >> vtIndexTemp >> slash >> vnIndexTemp;
				vIndexTemp--;
				vnIndexTemp--;
				vtIndexTemp--;
				triangle[i].position = v[vIndexTemp];
				triangle[i].normal = vn[vnIndexTemp];
				triangle[i].texCoord = vt[vtIndexTemp];
			}
			if (makeLeftHanded)
			{
				Vertex3D temp = triangle[0];
				triangle[0] = triangle[2];
				triangle[2] = temp;
			}


			for (int i = 0; i < 3; i++)
				shape.emplace_back(triangle[i]);
			
		}
	}

	if (FAILED(vbuffer.CreateVertexBuffer(pDevice, shape.data(), (unsigned int)shape.size())))
	{
		Popup::Error("Failed to create vertex buffer");
		return false;
	}
	return true;
}

Mesh::Mesh(std::string path, ID3D11Device*& device, bool makeLeftHanded)
{
	if (!LoadObjToBuffer(path, device, m_vbuffer, makeLeftHanded))
	{
		Popup::Error("loading " + path);
		throw;
	}
	m_name = StringHelper::GetName(path);
}

Buffer<Vertex3D>& Mesh::GetVBuffer()
{
	return m_vbuffer;
}

std::string Mesh::GetName() const
{
	return m_name;
}
