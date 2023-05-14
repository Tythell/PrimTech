#include <pch.h>
#include "Meshref.h"

namespace pt
{
	MeshRef::MeshRef(EntIdType entId, std::string meshName) : Component(entId)
	{
		PrimtTech::ResourceHandler::GetMesh(m_index).IncreaseUses(1);
		Init(meshName);
	}

	//MeshRef::MeshRef(const MeshRef& other): Component(other.EntId())
	//{
	//	m_pMesh = other.m_pMesh;
	//}

	void MeshRef::Init(std::string name)
	{
		m_instanceIndex = PrimtTech::ResourceHandler::GetMesh(m_index).IncreaseUses(-1);
		if (name == "")
		{
			m_pMaterialindexes.resize(1, 0);
			m_instanceIndex = PrimtTech::ResourceHandler::GetMesh(m_index).IncreaseUses(1);
			return;
		}
		//std::vector<PrimtTech::Mesh>& meshArr = PrimtTech::ResourceHandler::GetMeshArrayReference();

		int meshIndex = PrimtTech::ResourceHandler::CheckMeshNameExists(name);

		m_index = (meshIndex == -1) ? 0 : meshIndex;

		m_instanceIndex = PrimtTech::ResourceHandler::GetMesh(m_index).IncreaseUses(1);

		uint u = PrimtTech::ResourceHandler::GetMeshAdress(m_index)->GetNofMeshes();
		m_pMaterialindexes.resize(u, 0);

	}

	void MeshRef::SetIndex(uint idx)
	{
		m_index = idx;
	}

	void MeshRef::SetMaterial(uint materialIndex, uint slot)
	{
		m_pMaterialindexes[slot] = materialIndex;
	}

	void MeshRef::SetMaterial(std::string name, uint slot)
	{
		SetMaterial(std::max(0, PrimtTech::ResourceHandler::CheckMtrlNameExists(name)), slot);
	}
	uint MeshRef::GetMaterialIndex(const uint& index) const
	{
		if (m_pMaterialindexes.size() == 0)
			return 0;
		return m_pMaterialindexes[index];
	}
	uint MeshRef::GetInstIndex() const
	{
		return m_instanceIndex;
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
		m_index = otherComp->m_index;
		PrimtTech::ResourceHandler::GetMesh(m_index).IncreaseUses(1);
		m_pMaterialindexes = otherComp->m_pMaterialindexes;
	}
	void MeshRef::OnFree()
	{
		PrimtTech::ResourceHandler::GetMesh(m_index).IncreaseUses(-1);
	}
	void MeshRef::SetType(Type type)
	{
		m_type = type;
	}
	MeshRef::Type MeshRef::GetType() const
	{
		return m_type;
	}
	uint MeshRef::GetIndex() const
	{
		return m_index;
	}
	MeshPrefabRef::MeshPrefabRef(EntIdType entId):Component(entId)
	{
		PrimtTech::Prefab& p = PrimtTech::ResourceHandler::GetPrefab(m_prefabIndex);
		m_instanceIndex = p.IncreaseUses(1);
		p.AddRefIdx(0);

	}
	void MeshPrefabRef::SetPrefabIndex(uint i)
	{
		PrimtTech::Prefab& p = PrimtTech::ResourceHandler::GetPrefab(m_prefabIndex);
		p.IncreaseUses(-1);

		m_prefabIndex = i;
		m_instanceIndex = PrimtTech::ResourceHandler::GetPrefab(m_prefabIndex).IncreaseUses(1);
	}
	int MeshPrefabRef::Lua_ChangePrefab(lua_State* L)
	{
		throw;
		return 0;
	}
	void MeshPrefabRef::DuplicateFrom(Component* other)
	{
		MeshPrefabRef* o = dynamic_cast<MeshPrefabRef*>(other);
		PrimtTech::ResourceHandler::GetPrefab(m_prefabIndex).IncreaseUses(-1);

		m_prefabIndex = o->m_prefabIndex;
		m_instanceIndex = PrimtTech::ResourceHandler::GetPrefab(m_prefabIndex).IncreaseUses(1);
		//m_instanceIndex = o->m_instanceIndex;
	}
	void MeshPrefabRef::OnFree()
	{
		PrimtTech::ResourceHandler::GetPrefab(m_prefabIndex).IncreaseUses(-1);
	}
	void MeshPrefabRef::RefreshInstance()
	{
		SetPrefabIndex(m_prefabIndex);
	}
	uint MeshPrefabRef::GetIndex() const
	{
		return m_prefabIndex;
	}
	uint MeshPrefabRef::GetInstIndex() const
	{
		return m_instanceIndex;
	}
}