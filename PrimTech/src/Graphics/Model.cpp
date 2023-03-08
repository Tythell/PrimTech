#include"pch.h"
#include "Buffer.h"
#include"../Math/Math.h"
#include "Model.h"

namespace PrimtTech
{
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
		if (embeddedmatIndexes.size() == 0)
			for (int i = 0; i < embeddedmats.size(); i++)
			{
				std::string texturePath = embeddedmats[i].diffuseName;
				if (!texturePath.empty())
					LoadTexture(texturePath, i);
			}

		for (int i = 0; i < m_nOfMats; i++)
			m_material[i].SetLeftHanded(makeLeftHanded);
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

}


