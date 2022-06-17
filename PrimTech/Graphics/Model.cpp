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
	int meshIndex = ResourceHandler::CheckMeshNameExists(StringHelper::GetName(path));
	if (meshIndex != -1)
		mp_mesh = ResourceHandler::GetMeshAdress(meshIndex);
	else
		mp_mesh = ResourceHandler::AddMesh(path, pDevice);
	//LoadObjToBuffer(path, pDevice, m_vbuffer);
}

void Model::Draw()
{
	UINT offset = 0;
	dc->VSSetConstantBuffers(0, 1, mp_cbTransformBuffer->GetReference());
	mp_cbTransformBuffer->Data().world = GetWorldTransposed();
	mp_cbTransformBuffer->UpdateCB();
	dc->IASetVertexBuffers(0, 1, mp_mesh->GetVBuffer().GetReference(), mp_mesh->GetVBuffer().GetStrideP(), &offset);
	dc->Draw(mp_mesh->GetVBuffer().GetBufferSize(), 0);
}

bool LoadObjToBuffer(std::string path, ID3D11Device*& pDevice, Buffer<Vertex3D>& vbuffer)
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

	if (FAILED(vbuffer.CreateVertexBuffer(pDevice, shape.data(), (unsigned int)shape.size())))
	{
		Popup::Error("Failed to create vertex buffer");
		return false;
	}
	return true;
}

Mesh::Mesh(std::string path, ID3D11Device*& device)
{
	if (!LoadObjToBuffer(path, device, m_vbuffer))
		Popup::Error("loading model went wrong");
	m_name = StringHelper::GetName(path);
	//ResourceHandler::AddMesh(path, device);
}

Buffer<Vertex3D>& Mesh::GetVBuffer()
{
	return m_vbuffer;
}

std::string Mesh::GetName() const
{
	return m_name;
}

std::vector<Mesh*> ResourceHandler::m_meshes;

Mesh* ResourceHandler::AddMesh(std::string path, ID3D11Device*& pDevice)
{
	Mesh* pMesh = new Mesh(path, pDevice);
	m_meshes.emplace_back(pMesh);
	return pMesh;
}

Mesh& ResourceHandler::GetMesh(unsigned int index)
{
	return *m_meshes[index];
}

Mesh* ResourceHandler::GetMeshAdress(unsigned int index)
{
	return m_meshes[index];
}

int ResourceHandler::CheckMeshNameExists(std::string meshName)
{
	for (int i = 0; i < m_meshes.size(); i++)
	{
		if (m_meshes[i]->GetName() == meshName)
			return i;
	}
	return -1;
}

void ResourceHandler::Unload()
{
	for (int i = 0; i < m_meshes.size(); i++)
	{
		delete m_meshes[i];
	}
}
