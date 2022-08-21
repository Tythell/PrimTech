#include "Buffer.h"
#include"../Math/Math.h"
#include<vector>
#include "Model.h"
#include<iostream>
#include <fstream>
#include "ResourceHandler.h"
#include "../Utility/CommonDialogs.h"

Model::~Model()
{
	delete[] m_material;
}

void Model::Init(const std::string path, ModelType e, bool makeLeftHanded)
{
	m_type = e;
	std::string fullpath = "Assets/models/" + path;
	m_name = "";
	int meshIndex = ResourceHandler::CheckMeshNameExists(StringHelper::GetName(fullpath));
	std::vector<Mtl> embeddedmats;
	std::vector<int> embeddedmatIndexes;
	
	if (meshIndex != -1)
	{
		mp_mesh = ResourceHandler::GetMeshAdress(meshIndex);
		m_name += "(" + std::to_string(mp_mesh->GetNrOfUses()) + ")";
	}
	else
		mp_mesh = ResourceHandler::AddMesh(fullpath, makeLeftHanded);
	m_nOfMats = mp_mesh->GetNofMeshes();
	m_material = new Material[m_nOfMats];
	embeddedmats = mp_mesh->GetMtl();
	embeddedmatIndexes = mp_mesh->GetMtlIndex();
	for (int i = 0; i < embeddedmatIndexes.size(); i++)
	{
		std::string texturePath = embeddedmats[embeddedmatIndexes[i]].diffuseName;
		if (!texturePath.empty())
			LoadTexture(texturePath, i);
	}

	for (int i = 0; i < m_nOfMats; i++)
		m_material[i].SetLeftHanded(makeLeftHanded);
	
	mp_mesh->IncreaseUses();
	m_name += mp_mesh->GetName();

	if (e == ModelType::eDEBUG)
		m_material->SetRimColor(WHITE_3F);

	//dc->VSSetConstantBuffers(0, 1, mp_cbTransformBuffer->GetReference());
}

void Model::Draw()
{
	UINT offset = 0;

	mp_cbTransformBuffer->Data().world = GetWorldTransposed();
	mp_cbTransformBuffer->MapBuffer();
	dc->IASetVertexBuffers(0, 1, mp_mesh->GetVBuffer().GetReference(), mp_mesh->GetVBuffer().GetStrideP(), &offset);
	for (int i = 0; i < mp_mesh->GetNofMeshes(); i++)
	{
		m_material[i].GetBuffer()->Data().characterLight[0] = m_characterLight[0];
		m_material[i].Set(dc);
		int v1 = mp_mesh->GetMeshOffsfets()[i + 1], v2 = mp_mesh->GetMeshOffsfets()[i];
		dc->Draw(v1 - v2, v2);
 	}
}

void Model::UpdateTextureScroll(const float& deltatime)
{
	for (int i = 0; i < m_nOfMats; i++)
		m_material[i].UpdateTextureScroll(deltatime);
}

void Model::LoadTexture(std::string path, UINT i, TextureType type)
{
	m_material[i].LoadTexture(path, type);
}

void Model::SetLight(const sm::Vector4& v, const UINT& index)
{
	m_characterLight[index] = v;
}

void Model::SetMaterialBuffer(Buffer<hlsl::cbpMaterialBuffer>& cbMaterialBuffer)
{
	for (int i = 0; i < m_nOfMats; i++)
		m_material[i].SetPointers(&cbMaterialBuffer);
}

void Model::DecreaseMeshUsage()
{
	mp_mesh->DecreaseUses();
}

Material& Model::GetMaterial(const UINT& i)
{
	return m_material[i];
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

bool LoadObjToBuffer(std::string path, std::vector<Shape>& shape, std::vector<Mtl>& localMtls, std::vector<int>& matIndex, bool makeLeftHanded)
{
	//shape.resize(1);
	Shape localShape;
	UINT nofMats = 0;
	UINT currentMat = 0;

	//makeLeftHanded = false;

	std::string dummy;
	std::vector<sm::Vector3> v;
	std::vector<sm::Vector3> vn;
	std::vector<sm::Vector2> vt;

	std::vector<UINT> posIndex;
	std::vector<UINT> normalIndex;
	std::vector<UINT> texcoordsIndex;

	//std::vector<Mtl> mtls;

	std::ifstream reader(path);

	if (!reader.is_open())
		return false;
	while (std::getline(reader, dummy))
	{
		std::string input;
		//std::cout << s;
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
					localShape.verts.emplace_back(triangle[i]);
			else
				for (int i = 0; i < 3; i++)
					localShape.verts.emplace_back(triangle[i]);

		}
		else if (input == "g")
		{
			std::string sgname;
			reader >> sgname;
			if (!localShape.verts.empty())
			{
				shape.emplace_back(localShape);
				localShape.verts.clear();
			}
		}
		else if (input == "usemtl")
		{
			std::string sgname;
			reader >> sgname;

			if (!localShape.verts.empty())
			{
				shape.emplace_back(localShape);
				localShape.verts.clear();
			}
			
			for (int i = 0; i < localMtls.size(); i++)
			{
				if (localMtls[i].name == sgname)
					matIndex.emplace_back(i);
			}

		}
		else if (input == "mtllib")
		{
			std::string matname;
			reader >> matname;
			std::ifstream matreader("Assets/models/" + matname);
			if (matreader.is_open())
			{
				std::string sdummy;
				while (std::getline(matreader, sdummy))
				{
					if (sdummy[0] == 'n' && sdummy[1] == 'e')
					{
						Mtl mtl;
						nofMats++;
						input = sdummy.substr(7);
						mtl.name = input;
						//mtls.emplace_back(mtl);
						localMtls.resize(localMtls.size() + 1);
						localMtls[localMtls.size() - 1] = mtl;
					}
					matreader >> input;
					if (input == "newmtl")
					{
						Mtl mtl;
						nofMats++;
						matreader >> input;
						mtl.name = input;
						localMtls.resize(localMtls.size() + 1);
						localMtls[localMtls.size() - 1] = mtl;
						//mtls.emplace_back(mtl);
					}
					else if (input == "map_Kd")
					{
						matreader >> input;
						localMtls[nofMats - 1].diffuseName = input;
					}
					
				}
				matreader.close();
			}
		}
		
	}
	shape.emplace_back(localShape);
	for (int si = 0; si < shape.size(); si++)
	{
		for (int i = 0; i < shape[si].verts.size(); i += 3)
		{
			sm::Vector2 UVA = shape[si].verts[i + 0].texCoord;
			sm::Vector2 UVB = shape[si].verts[i + 1].texCoord;
			sm::Vector2 UVC = shape[si].verts[i + 2].texCoord;

			sm::Vector3 POSA = shape[si].verts[i + 0].position;
			sm::Vector3 POSB = shape[si].verts[i + 1].position;
			sm::Vector3 POSC = shape[si].verts[i + 2].position;

			sm::Vector2 dAB = UVB - UVA;
			sm::Vector2 dAC = UVC - UVA;

			sm::Vector3 edge1 = POSB - POSA;
			sm::Vector3 edge2 = POSC - POSA;

			float f = 1.0f / (dAB.x * dAC.y - dAC.x * dAB.y);

			sm::Vector3 tangent;
			tangent.x = f * (dAC.y * edge1.x - dAB.y * edge2.x);
			tangent.y = f * (dAC.y * edge1.y - dAB.y * edge2.y);
			tangent.z = f * (dAC.y * edge1.z - dAB.y * edge2.z);

			shape[si].verts[i].tangent = tangent;
			shape[si].verts[i + 1].tangent = tangent;
			shape[si].verts[i + 2].tangent = tangent;
		}

		//if(!localMtls.empty())
		//	shape[si].material = localMtls[si];
	}
	
	//localShape.clear();
  	reader.close();
	return true;
}

Mesh::Mesh(std::string path, ID3D11Device*& device, bool makeLeftHanded)
{
	std::vector<Shape> mesh;

	if (!LoadObjToBuffer(path, mesh, m_mtls, m_mtlIndexes, makeLeftHanded))
	{
		Popup::Error("loading " + path);
		throw;
	}
	m_name = StringHelper::GetName(path);
	m_nofMeshes = mesh.size();
	m_offsets.emplace_back(0);
	int lastSize = 0;
	for (int i = 0; i < m_nofMeshes; i++)
	{
		lastSize += mesh[i].verts.size();
		m_offsets.emplace_back(lastSize);
	}

	std::vector<d::XMFLOAT3> positionArray;
	//Shape fullshape;
	UINT totalVertCount = 0;
	for (int i = 0; i < m_nofMeshes; i++)
	{
		for (int j = 0; j < mesh[i].verts.size(); j++)
		{
			sm::Vector3 pos = mesh[i].verts[j].position;
			positionArray.emplace_back(pos);
			mesh[i].verts[j].color = sm::Vector3(1.f, 1.f, 1.f);
			//if(rand() % 2 == 0)
				mesh[i].verts[j].color = sm::Vector3(float(rand() % 10 +1) / 10, float(rand() % 10 + 1) / 10, float(rand() % 10+1) / 10);
			m_shape.verts.emplace_back(mesh[i].verts[j]);
		}
	}
	UINT bsize = m_shape.verts.size();
	d::BoundingBox box;
	d::BoundingBox::CreateFromPoints(box, positionArray.size(), positionArray.data(), sizeof(sm::Vector3));
	d::BoundingSphere::CreateFromBoundingBox(m_bsphere, box);

	HRESULT hr = m_vbuffer.CreateVertexBuffer(device, m_shape.verts.data(), bsize);
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

std::vector<Mtl> Mesh::GetMtl() const
{
	return m_mtls;
}

std::vector<int> Mesh::GetMtlIndex() const
{
	return m_mtlIndexes;
}

std::vector<int> Mesh::GetMeshOffsfets() const
{
	return m_offsets;
}
