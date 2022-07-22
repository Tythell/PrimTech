#include "Buffer.h"
#include"../Math/Math.h"
#include<vector>
#include "Model.h"
#include<iostream>
#include <fstream>
#include "ResourceHandler.h"
#include "../Utility/CommonDialogs.h"

Model::Model()
{
	//AllModels::AddModelAdress(this);
}
//
////ID3D11DeviceContext* Model::dc;
////Buffer<hlsl::cbpWorldTransforms3D>* Model::mp_cbTransformBuffer;
//std::vector<Model*> AllModels::m_models;

void Model::Init(const std::string path, ModelType e, bool makeLeftHanded)
{
	m_material.SetLeftHanded(makeLeftHanded);
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

	m_material.Set(dc);
	mp_cbTransformBuffer->Data().world = GetWorldTransposed();
	mp_cbTransformBuffer->UpdateBuffer();
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

d::BoundingBox Model::GetBBox() const
{
	return mp_mesh->GetBBox();
}

bool LoadObjToBuffer(std::string path, std::vector<Vertex3D>& shape, bool makeLeftHanded)
{
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

			if(makeLeftHanded)
				for (int i = 2; i > -1; i--)
					shape.emplace_back(triangle[i]);
			else
				for (int i = 0; i < 3; i++)
					shape.emplace_back(triangle[i]);

		}

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
	}
	return true;
}

Mesh::Mesh(std::string path, ID3D11Device*& device, bool makeLeftHanded)
{
	std::vector<Vertex3D> vertexes;

	if (!LoadObjToBuffer(path, vertexes, makeLeftHanded))
	{
		Popup::Error("loading " + path);
		throw;
	}
	m_name = StringHelper::GetName(path);


	std::vector<d::XMFLOAT3> positionArray;
	positionArray.resize(vertexes.size());
	for (int i = 0; i < vertexes.size(); i++)
		positionArray[i] = vertexes[i].position;
	d::BoundingBox::CreateFromPoints(m_bbox, vertexes.size(), positionArray.data(), sizeof(sm::Vector3));

	HRESULT hr = m_vbuffer.CreateVertexBuffer(device, vertexes.data(), vertexes.size());
	if (FAILED(hr))
		COM_ERROR(hr, "Failed to load vertex buffer");
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

void Mesh::ResetUses()
{
	m_nrOfUses = 0;
}

int Mesh::GetNrOfUses() const
{
	return m_nrOfUses;
}

d::BoundingBox Mesh::GetBBox() const
{
	return m_bbox;
}

//void AllModels::SetBuffers(ID3D11DeviceContext*& dc, Buffer<hlsl::cbpWorldTransforms3D>& buffer, Buffer<hlsl::cbpMaterialBuffer>& matBuffer)
//{
//	for (int i = 0; i < m_models.size(); i++)
//	{
//		m_models[i]->SetDCandBuffer(dc, buffer);
//		m_models[i]->SetMaterialBuffer(matBuffer);
//	}
//}

//void AllModels::AddModelAdress(Model* pm)
//{
//	m_models.emplace_back(pm);
//}

//void AllModels::SetNamesToVector(std::vector<std::string>& v)
//{
	//int hej = m_models.size();
	//hej++;
	//v.resize(m_models.size());
	//for (int i = 0; i < v.size(); i++)
	//{
	//	if (m_models[i]->GetModelType() != ModelType::eUNSPECIFIED)
	//		v[i] = "DEBUG| ";
	//	v[i] += m_models[i]->GetName();
	//}
//}

//int AllModels::GetNrOfModels()
//{
//	return (int)m_models.size();
//}
//
//Model* AllModels::GetModel(int index)
//{
//	return m_models[index];
//}
//
//bool AllModels::ExportScene(std::string path)
//{
//	Sceneheaders header = Sceneheaders::eMODEL;
//
//	std::ofstream writer(path, std::ios::binary | std::ios::out);
//	for (int i = 0; i < m_models.size(); i++)
//	{
//		ModelStruct ms;
//		strcpy_s(ms.modelname, m_models[i]->GetName().c_str());
//		ms.scale = m_models[i]->GetScale();
//		ms.rotation = m_models[i]->GetRotation();
//		ms.position = m_models[i]->GetPosition();
//		strcpy_s(ms.mtrlname, m_models[i]->GetMaterial().GetFileName().c_str());
//		writer.write((const char*)&header, 4);
//		writer.write((const char*)&ms, sizeof(ModelStruct));
//	}
//	header = Sceneheaders::enull;
//	writer.write((const char*)&header, 4);
//	writer.close();
//	return true;
//}

void RenderBox::Draw(ID3D11DeviceContext*& dc)
{
	UINT offset = 0;
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	
	dc->IASetVertexBuffers(0, 1, m_vbuffer.GetReference(), m_vbuffer.GetStrideP(), &offset);
	dc->IASetIndexBuffer(m_ibuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	dc->DrawIndexed(m_ibuffer.GetBufferSize(), 0, 0);
}

Buffer<BBVertex>& RenderBox::GetVBuffer()
{
	return m_vbuffer;
}

void RenderBox::Init(ID3D11Device*& device)
{
	BBVertex vertex[]
	{
		{{-0.5f, 0.5f, -0.5f }, WHITE_3F}, //F TL	 0
		{{0.5f, 0.5f, -0.5f }, WHITE_3F}, //F TR	 1
		{{-0.5f, -0.5f, -0.5f }, WHITE_3F}, //F BL 2
		{{0.5f, -0.5f, -0.5f }, WHITE_3F}, //F BR	 3
		{{-0.5f, 0.5f, 0.5f }, WHITE_3F}, //B TL	 4
		{{0.5f, 0.5f, 0.5f }, WHITE_3F}, //B TR	 5
		{{-0.5f, -0.5f, 0.5f }, WHITE_3F}, //B BL	 6
		{{0.5f, -0.5f, 0.5f }, WHITE_3F}, //B BR	 7
	};

	unsigned int indexes[]
	{
		1, 0, 0, 2, 2, 3, 3, 1, // front square
		5, 4, 4, 6, 6, 7, 7, 5, // back square
		0, 4, 1, 5, 2, 6, 3, 7, // connectors
	};

	m_vbuffer.CreateVertexBuffer(device, vertex, ARRAYSIZE(vertex));
	m_ibuffer.CreateIndexBuffer(device, indexes, ARRAYSIZE(indexes));
}

void RenderBox::SetBox(d::BoundingBox box)
{
}

void RenderLine::Draw(ID3D11DeviceContext*& dc)
{
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, m_vbuffer.GetReference(), m_vbuffer.GetStrideP(), &offset);
	dc->Draw(2, 0);
}

void RenderLine::Init(ID3D11Device*& device, ID3D11DeviceContext*& dc)
{
	BBVertex line[]
	{
		{{0.f, 0.f, -.5f}, WHITE_3F},
		{{0.f, 0.f, .5f}, BLUE_3F},
	};
	m_vbuffer.CreateVertexBuffer(device, line, ARRAYSIZE(line), dc);
}

void RenderLine::SetLine(sm::Vector3 start, sm::Vector3 end)
{
	m_vbuffer.ArrData(0).m_position = start;
	m_vbuffer.ArrData(0).m_color = WHITE_3F;
	m_vbuffer.ArrData(1).m_position = end;
	m_vbuffer.ArrData(1).m_color = BLUE_3F;
	m_vbuffer.UpdateBuffer();
}
