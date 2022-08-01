#include "Buffer.h"
#include"../Math/Math.h"
#include<vector>
#include "Model.h"
#include<iostream>
#include <fstream>
#include "ResourceHandler.h"
#include "../Utility/CommonDialogs.h"

//Model::Model()
//{
	//AllModels::AddModelAdress(this);
//}
//
////ID3D11DeviceContext* Model::dc;
////Buffer<hlsl::cbpWorldTransforms3D>* Model::mp_cbTransformBuffer;
//std::vector<Model*> AllModels::m_models;

Model::Model()
{
}

Model::~Model()
{
}

void Model::Init(const std::string path, ModelType e, bool makeLeftHanded)
{
	m_material.SetLeftHanded(makeLeftHanded);
	m_type = e;
	std::string fullpath = "Assets/models/" + path;
	m_name = "";
	int meshIndex = ResourceHandler::CheckMeshNameExists(StringHelper::GetName(fullpath));
	if (meshIndex != -1)
	{
		mp_mesh = ResourceHandler::GetMeshAdress(meshIndex);
		m_name += "(" + std::to_string(mp_mesh->GetNrOfUses()) + ")";
	}
	else
		mp_mesh = ResourceHandler::AddMesh(fullpath, makeLeftHanded);
	mp_mesh->IncreaseUses();
	m_name += mp_mesh->GetName();

	if (e == ModelType::eDEBUG)
		m_material.SetRimColor(RED_3F);

	//dc->VSSetConstantBuffers(0, 1, mp_cbTransformBuffer->GetReference());
}

void Model::Draw()
{
	UINT offset = 0;

	m_material.GetBuffer()->Data().characterLight[0] = m_characterLight[0];
	m_material.Set(dc);


	mp_cbTransformBuffer->Data().world = GetWorldTransposed();
	mp_cbTransformBuffer->UpdateBuffer();
	for (int i = 0; i < mp_mesh->GetNofMeshes(); i++)
	{
		dc->IASetVertexBuffers(0, 1, mp_mesh->GetVBuffer(i).GetReference(), mp_mesh->GetVBuffer().GetStrideP(), &offset);
		dc->Draw(mp_mesh->GetVBuffer(i).GetBufferSize(), 0);
	}
}

void Model::UpdateTextureScroll(const float& deltatime)
{
	m_material.UpdateTextureScroll(deltatime);
}

void Model::LoadTexture(std::string path, TextureType type)
{
	m_material.LoadTexture(path, type);
}

void Model::SetLight(const sm::Vector4& v, const UINT& index)
{
	m_characterLight[index] = v;
}

void Model::SetMaterialBuffer(Buffer<hlsl::cbpMaterialBuffer>& cbMaterialBuffer)
{
	m_material.SetPointers(&cbMaterialBuffer);
}

void Model::DecreaseMeshUsage()
{
	mp_mesh->DecreaseUses();
}

Material& Model::GetMaterial()
{
	return m_material;
}

Mesh* Model::GetMeshP()
{
	return mp_mesh;
}

std::string Model::GetName() const
{
	return m_name;
}

void Model::SetDCandBuffer(ID3D11DeviceContext*& pdc, Buffer<hlsl::cbpWorldTransforms3D>& pCbuffer)
{
	dc = pdc;
	mp_cbTransformBuffer = &pCbuffer;
}

ModelType Model::GetModelType() const
{
	return m_type;
}

d::BoundingSphere Model::GetBSphere() const
{
	return mp_mesh->GetBSphere();
}

const sm::Vector4 Model::GetCharacterLight(int i) const
{
	return m_characterLight[i];
}

bool LoadObjToBuffer(std::string path, std::vector<Shape>& shape, bool makeLeftHanded)
{
	//shape.resize(1);
	Shape localShape;
	//int nofG = 0;

	//makeLeftHanded = false;
	std::string s;
	std::vector<sm::Vector3> v;
	std::vector<sm::Vector3> vn;
	std::vector<sm::Vector2> vt;

	std::vector<UINT> posIndex;
	std::vector<UINT> normalIndex;
	std::vector<UINT> texcoordsIndex;

	//std::vector<Vertex3D> shape;

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
			DirectX::XMFLOAT3 vertex;
			reader >> vertex.x >> vertex.y >> vertex.z;
			if (makeLeftHanded) vertex.z *= -1;
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
			if (makeLeftHanded) normal.z *= -1;
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
				for (int i = 2; i > -1; i--)
					localShape.emplace_back(triangle[i]);
			else
				for (int i = 0; i < 3; i++)
					localShape.emplace_back(triangle[i]);

		}
		else if (input == "g")
		{
			std::string sgname;
			reader >> sgname;
			if (!localShape.empty())
			{
				shape.emplace_back(localShape);
				localShape.clear();
			}
			//nofG++;
		}
		else if (input == "usemtl")
		{
			std::string sgname;
			reader >> sgname;
			if (!localShape.empty())
			{
				shape.emplace_back(localShape);
				localShape.clear();
			}
			//nofG++;
		}
		
	}
	shape.emplace_back(localShape);
	for (int si = 0; si < shape.size(); si++)
	{
		for (int i = 0; i < shape[si].size(); i += 3)
		{
			sm::Vector2 UVA = shape[si][i + 0].texCoord;
			sm::Vector2 UVB = shape[si][i + 1].texCoord;
			sm::Vector2 UVC = shape[si][i + 2].texCoord;

			sm::Vector3 POSA = shape[si][i + 0].position;
			sm::Vector3 POSB = shape[si][i + 1].position;
			sm::Vector3 POSC = shape[si][i + 2].position;

			sm::Vector2 dAB = UVB - UVA;
			sm::Vector2 dAC = UVC - UVA;

			sm::Vector3 edge1 = POSB - POSA;
			sm::Vector3 edge2 = POSC - POSA;

			float f = 1.0f / (dAB.x * dAC.y - dAC.x * dAB.y);

			sm::Vector3 tangent;
			tangent.x = f * (dAC.y * edge1.x - dAB.y * edge2.x);
			tangent.y = f * (dAC.y * edge1.y - dAB.y * edge2.y);
			tangent.z = f * (dAC.y * edge1.z - dAB.y * edge2.z);

			shape[si][i].tangent = tangent;
			shape[si][i + 1].tangent = tangent;
			shape[si][i + 2].tangent = tangent;
		}
	}
		
	
	//localShape.clear();
	reader.close();
	return true;
}

Mesh::Mesh(std::string path, ID3D11Device*& device, bool makeLeftHanded)
{
	std::vector<Shape> vertexes;

	if (!LoadObjToBuffer(path, vertexes, makeLeftHanded))
	{
		Popup::Error("loading " + path);
		throw;
	}
	m_name = StringHelper::GetName(path);
	m_nofMeshes = vertexes.size();

	std::vector<d::XMFLOAT3> positionArray;
	UINT totalVertCount = 0;
	for (int i = 0; i < m_nofMeshes; i++)
	{
		for (int j = 0; j < vertexes[i].size(); j++)
		{
			sm::Vector3 pos = vertexes[i][j].position;
			positionArray.emplace_back(pos);
		}
	}
	//positionArray.resize(vertexes[0].size());


	d::BoundingBox box;
	d::BoundingBox::CreateFromPoints(box, positionArray.size(), positionArray.data(), sizeof(sm::Vector3));
	d::BoundingSphere::CreateFromBoundingBox(m_bsphere, box);

	m_vbuffer.resize(m_nofMeshes);
	for (int i = 0; i < m_nofMeshes; i++)
	{
		HRESULT hr = m_vbuffer[i].CreateVertexBuffer(device, vertexes[i].data(), vertexes[i].size());
		COM_ERROR(hr, "Failed to load vertex buffer");
	}
	
}

Buffer<Vertex3D>& Mesh::GetVBuffer(const UINT& index)
{
	return m_vbuffer[index];
}

std::string Mesh::GetName() const
{
	return m_name;
}

void Mesh::IncreaseUses()
{
	m_nrOfUses++;
}

void Mesh::DecreaseUses()
{
	if (m_nrOfUses > 0) m_nrOfUses--;
}

void Mesh::ResetUses()
{
	m_nrOfUses = 0;
}

int Mesh::GetNrOfUses() const
{
	return m_nrOfUses;
}

d::BoundingSphere Mesh::GetBSphere() const
{
	return m_bsphere;
}

const UINT Mesh::GetNofMeshes() const
{
	return m_nofMeshes;
}
