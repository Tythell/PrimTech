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
		//std::vector<PrimtTech::Mesh>& meshArr = PrimtTech::ResourceHandler::GetMeshArrayReference();

		int meshIndex = PrimtTech::ResourceHandler::CheckMeshNameExists(name);

		m_meshIndex = (meshIndex == -1) ? 0 : meshIndex;

		m_pMaterialindexes.resize(PrimtTech::ResourceHandler::GetMeshAdress(m_meshIndex)->GetNofMeshes(), 0);

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

