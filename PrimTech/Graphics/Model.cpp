#include "Buffer.h"
#include"../Math/Math.h"
#include<vector>
#include "Model.h"
#include<iostream>
#include <fstream>
#include "ResourceHandler.h"

Model::Model()
{
	AllModels::AddModelAdress(this);
}

//ID3D11DeviceContext* Model::dc;
//Buffer<hlsl::cbpWorldTransforms3D>* Model::mp_cbTransformBuffer;
std::vector<Model*> AllModels::m_models;

void Model::Init(const std::string path, ModelType e, bool makeLeftHanded)
{
	m_type = e;
	std::string fullpath = "Assets/models/" + path;
	//dc = pDc;
	//mp_cbTransformBuffer = &buffer;
	m_name = "";
	int meshIndex = ResourceHandler::CheckMeshNameExists(StringHelper::GetName(fullpath));
	if (meshIndex != -1)
	{
		mp_mesh = ResourceHandler::GetMeshAdress(meshIndex);
		m_name += "(" + std::to_string(mp_mesh->GetNrOfUses()) + ")";
	}
	else
		mp_mesh = ResourceHandler::AddMesh(fullpath);
	mp_mesh->IncreaseUses();
	m_name += mp_mesh->GetName();

	if (e == ModelType::eDEBUG)
		m_material.SetRimColor(RED_3F);

	//dc->VSSetConstantBuffers(0, 1, mp_cbTransformBuffer->GetReference());
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

void Model::SetMaterialBuffer(Buffer<hlsl::cbpMaterialBuffer>& cbMaterialBuffer)
{
	m_material.SetPointers(&cbMaterialBuffer);
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

bool LoadObjToBuffer(std::string path, ID3D11Device*& pDevice, Buffer<Vertex3D>& vbuffer, bool makeLeftHanded)
{
	std::string s;
	std::vector<sm::Vector3> v;
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

			//for (uint32_t i = 0; i < shape.size(); i += 3u) {
			//	sm::Vector3 pos0 = shape[i].position;
			//	sm::Vector3 pos1 = shape[i + 1u].position;
			//	sm::Vector3 pos2 = shape[i + 2u].position;

			//	sm::Vector2 uv0 = shape[i].texCoord;
			//	sm::Vector2 uv1 = shape[i + 1u].texCoord;
			//	sm::Vector2 uv2 = shape[i + 2u].texCoord;

			//	sm::Vector2 duv1 = uv1 - uv0;
			//	sm::Vector2 duv2 = uv2 - uv0;
			//	sm::Vector3 de1 = pos1 - pos0;
			//	sm::Vector3 de2 = pos2 - pos0;

			//	float f = 1.0f / (duv1.x * duv2.y - duv2.x * duv1.y);

			//	sm::Vector3 t;
			//	sm::Vector3 bt;

			//	t.x = f * (duv2.y * de1.x - duv1.y * de2.x);
			//	t.y = f * (duv2.y * de1.y - duv1.y * de2.y);
			//	t.z = f * (duv2.y * de1.z - duv1.y * de2.z);

			//	bt.x = f * (-duv2.x * de1.x + duv1.x * de2.x);
			//	bt.y = f * (-duv2.x * de1.y + duv1.x * de2.y);
			//	bt.z = f * (-duv2.x * de1.z + duv1.x * de2.z);

			//	shape[i].tangent = (t - shape[i].normal * (shape[i].normal * t));
			//	shape[i + 1u].tangent = (t - shape[i + 1u].normal * (shape[i + 1u].normal * t));
			//	shape[i + 2u].tangent = (t - shape[i + 2u].normal * (shape[i + 2u].normal * t));

			////	//shape[i].bi = (bt - mesh[i].norm * (mesh[i].norm * bt));
			////	//shape[i + 1u].bi = (bt - mesh[i + 1u].norm * (mesh[i + 1u].norm * bt)).Normalized();
			////	//shape[i + 2u].bi = (bt - mesh[i + 2u].norm * (mesh[i + 2u].norm * bt)).Normalized();
			//}

			for (int i = 0; i < shape.size(); i += 3)
			{
				sm::Vector2 UVA = shape[i].texCoord;
				sm::Vector2 UVB = shape[i + 1].texCoord;
				sm::Vector2 UVC = shape[i + 2].texCoord;

				sm::Vector3 POSA = shape[i].position;
				sm::Vector3 POSB = shape[i + 1].position;
				sm::Vector3 POSC = shape[i + 2].position;

				sm::Vector2 dAB = UVB - UVA;
				sm::Vector2 dAC = UVC - UVA;

				sm::Vector3 edge1 = POSB - POSA;
				sm::Vector3 edge2 = POSC - POSA;

				float f = 1.0f / (dAB.x * dAC.y - dAC.x * dAB.y);

				sm::Vector3 tangent;
				tangent.x = f * (dAC.y * edge1.x - dAB.y * edge2.x);
				tangent.y = f * (dAC.y * edge1.y - dAB.y * edge2.y);
				tangent.z = f * (dAC.y * edge1.z - dAB.y * edge2.z);

				shape[i].tangent = tangent;
				shape[i + 1].tangent = tangent;
				shape[i + 2].tangent = tangent;
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

void Mesh::IncreaseUses()
{
	m_nrOfUses++;
}

int Mesh::GetNrOfUses() const
{
	return m_nrOfUses;
}

void AllModels::SetBuffers(ID3D11DeviceContext*& dc, Buffer<hlsl::cbpWorldTransforms3D>& buffer)
{
	for (int i = 0; i < m_models.size(); i++)
	{
		m_models[i]->SetDCandBuffer(dc, buffer);
	}
}

void AllModels::AddModelAdress(Model* pm)
{
	m_models.emplace_back(pm);
}

void AllModels::SetNamesToVector(std::vector<std::string>& v)
{
	v.resize(m_models.size());
	for (int i = 0; i < v.size(); i++)
	{
		if (m_models[i]->GetModelType() != ModelType::eUNSPECIFIED)
			v[i] = "DEBUG| ";
		v[i] += m_models[i]->GetName();
	}
}

int AllModels::GetNrOfModels()
{
	return (int)m_models.size();
}

Model* AllModels::GetModel(int index)
{
	return m_models[index];
}
