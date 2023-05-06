#include <pch.h>
#include "Meshref.h"

namespace pt
{
	MeshRef::MeshRef(EntIdType entId, std::string meshName) : Component(entId)
	{
		PrimtTech::ResourceHandler::GetMesh(m_meshIndex).IncreaseUses(1);
		Init(meshName);
	}

	//MeshRef::MeshRef(const MeshRef& other): Component(other.EntId())
	//{
	//	m_pMesh = other.m_pMesh;
	//}

	void MeshRef::Init(std::string name)
	{
		PrimtTech::ResourceHandler::GetMesh(m_meshIndex).IncreaseUses(-1);
		if (name == "")
		{
			m_pMaterialindexes.resize(1, 0);
			PrimtTech::ResourceHandler::GetMesh(m_meshIndex).IncreaseUses(1);
			return;
		}
		//std::vector<PrimtTech::Mesh>& meshArr = PrimtTech::ResourceHandler::GetMeshArrayReference();

		int meshIndex = PrimtTech::ResourceHandler::CheckMeshNameExists(name);

		m_meshIndex = (meshIndex == -1) ? 0 : meshIndex;

		PrimtTech::ResourceHandler::GetMesh(m_meshIndex).IncreaseUses(1);

		uint u = PrimtTech::ResourceHandler::GetMeshAdress(m_meshIndex)->GetNofMeshes();
		m_pMaterialindexes.resize(u, 0);

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
	int MeshRef::Lua_ChangeModel(lua_State* L)
	{
		MeshRef* m = (MeshRef*)lua_touserdata(L, 1);
		std::string meshName = lua_tostring(L, 2);
		m->Init(meshName);
		return 0;
	}
	void MeshRef::DuplicateFrom(Component* other)
	{
		MeshRef* otherComp = dynamic_cast<MeshRef*>(other);
		PrimtTech::ResourceHandler::GetMesh(0).IncreaseUses(-1);
		m_meshIndex = otherComp->m_meshIndex;
		PrimtTech::ResourceHandler::GetMesh(m_meshIndex).IncreaseUses(1);
		m_pMaterialindexes = otherComp->m_pMaterialindexes;
	}
	void MeshRef::OnFree()
	{
		PrimtTech::ResourceHandler::GetMesh(m_meshIndex).IncreaseUses(-1);
	}
}