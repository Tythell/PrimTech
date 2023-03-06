#include "pch.h"
#include "Component.h"

namespace pt
{
	Component::Component(const uint id) : m_entId(id)
	{}

	const uint Component::EntId() const
	{
		return m_entId;
	}

	MeshRef::MeshRef(uint entId, std::string meshName) : Component(entId)
	{
		Init(meshName);
	}

	//MeshRef::MeshRef(const MeshRef& other): Component(other.EntId())
	//{
	//	m_pMesh = other.m_pMesh;
	//}

	void MeshRef::Init(std::string name)
	{
		if (name == "")
		{
			return;
		}
		std::vector<PrimtTech::Mesh>& meshArr = PrimtTech::ResourceHandler::GetMeshArrayReference();

		for (int i = 0; i < meshArr.size(); i++)
		{
			if (meshArr[i].GetName() == name)
			{
				m_pMesh = &meshArr[i];
				m_pMaterialindexes.resize(m_pMesh->GetNofMeshes());
				return;
			}
		}
		m_pMaterialindexes.emplace_back(0u);
		m_pMesh = &meshArr[0];
	}

	void MeshRef::SetMaterial(uint materialIndex, uint slot)
	{
		m_pMaterialindexes[slot] = materialIndex;
	}

	void MeshRef::SetMaterial(std::string name, uint slot)
	{
		SetMaterial(PrimtTech::ResourceHandler::CheckMtrlNameExists(name), slot);
	}
	uint MeshRef::GetMaterialIndex(const uint& index) const
	{
		if (m_pMaterialindexes.size() == 0)
			return 0;
		return m_pMaterialindexes[index];
	}
}

