#include "pch.h"
#include "Entity.h"
//#include "ComponentHandler.h"

namespace pt
{
	uint Entity::nrOfEntities = 0;
	uint Entity::nrOfEntitiesUsed = 0;

	std::vector<Entity> Entity::s_ents;

	Entity::Entity(bool createdWithFunc)
	{
		m_id = nrOfEntitiesUsed++;
		nrOfEntities++;
		THROW_POPUP_ERROR(createdWithFunc, "MANNEN ANVÄND pt::Entity::Create() ISTÄLLET");
		AddComponent<TransformComp>();
		m_displayName = std::to_string(m_id);
	}

	Entity& Entity::Create(std::string name)
	{
		if (nrOfEntities - nrOfEntitiesUsed > 0)
		{
			int num = nrOfEntitiesUsed;
			//s_ents[num] = pt::Entity(true);
			
			//s_ents[num].AddComponent<TransformComp>();
			s_ents[num].SetName(name);
			if (name == "") s_ents[num].SetName(std::to_string(nrOfEntitiesUsed));
			nrOfEntitiesUsed++;
			return s_ents[num];
		}
		else
		{
			s_ents.emplace_back(true).SetName(name)/*.AddComponent<TransformComp>()*/;
			return s_ents[s_ents.size() - 1]; // Don't question it
		}
		
	}

	void Entity::Free()
	{
		--nrOfEntitiesUsed;
		FreeComponent<MeshRef>();
		FreeComponent<AABBComp>();
		FreeComponent<BSphereComp>();
		FreeComponent<Camera>();
		FreeComponent<Light>();
		FreeComponent<LuaScript>();
		FreeComponent<OBBComp>();
		FreeComponent<PhysicsBody>();

		FreeComponent<TransformComp>(m_id);

		std::swap(*this, s_ents[nrOfEntitiesUsed]);
	}

	const uint Entity::NumEnts()
	{
		return (uint)s_ents.size();
	}

	Entity& Entity::GetEntity(EntIdType index)
	{
		return s_ents[index];
	}
	Entity* Entity::GetEntityP(EntIdType index)
	{
		return &s_ents[index];
	}

	void Entity::Clear(uint until)
	{
		s_ents.resize((uint)until);
	}

	std::vector<Entity>& Entity::GetAllEnts()
	{
		return s_ents;
	}

	void Entity::ReserveEnts(size_t size)
	{
		s_ents.reserve(size);
	}

	void Entity::ClearAll()
	{
		//for (size_t i = 0; i < length; i++)
		//{

		//}
	}

	void Entity::SetPhysBodyIndex(int idx)
	{
		m_physIndex = idx;
	}

	void Entity::SetPosition(float x, float y, float z)
	{
		SetPosition(sm::Vector3(x, y, z));
	}

	void Entity::SetPosition(const sm::Vector3& v)
	{
		if (m_physIndex != -1)
		{
			PhysicsBody& physBod = PrimtTech::ComponentHandler::GetComponentByIndex<PhysicsBody>((uint)m_physIndex);

			physBod.SetPhysicsPosition(v);
		}
		else
			Transform().SetPosition(v);
	}

	void Entity::SetRotation(float x, float y, float z)
	{
		SetRotation(sm::Vector3(x, y, z));
	}

	void Entity::SetRotation(const sm::Vector3& v)
	{
		if (m_physIndex != -1)
		{
			PhysicsBody& physBod = PrimtTech::ComponentHandler::GetComponentByIndex<PhysicsBody>((uint)m_physIndex);

			physBod.SetPhysicsEulerRotation(v);
		}
		else
			Transform().SetRotation(v);
	}

	void Entity::SetRotation(const sm::Quaternion& v)
	{
		Transform().SetRotation(v);
	}

	void Entity::SetScale(float x, float y, float z)
	{
		SetScale(sm::Vector3(x,y,z));
	}

	void Entity::SetScale(const sm::Vector3& v)
	{
		Transform().SetScale(v);
	}

	void Entity::Move(float x, float y, float z)
	{
		Move(sm::Vector3(x, y, z));
	}

	void Entity::Move(sm::Vector3 v)
	{
		if (m_physIndex != -1)
		{
			PhysicsBody& physBod = PrimtTech::ComponentHandler::GetComponentByIndex<PhysicsBody>((uint)m_physIndex);

			physBod.PhysMove(v);
		}
		else
			Transform().Move(v);
	}

	void Entity::Rotate(float x, float y, float z)
	{
	}

	void Entity::Rotate(sm::Vector3 v)
	{
	}

	void Entity::Scale(float x, float y, float z)
	{
	}

	void Entity::Scale(sm::Vector3 v)
	{
	}

	int Entity::Lua_SetPosition(lua_State* L)
	{
		lua_getglobal(L, "Ent");
		lua_getfield(L, -1, "ptr");
		Entity* pEnt = (Entity*)lua_touserdata(L, -1);
		pEnt->SetPosition(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
		return 0;
	}

	int Entity::Lua_SetRotation(lua_State* L)
	{
		lua_getglobal(L, "Ent");
		lua_getfield(L, -1, "ptr");
		Entity* pEnt = (Entity*)lua_touserdata(L, -1);
		pEnt->SetRotation(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
		return 0;
	}

	int Entity::Lua_SetScale(lua_State* L)
	{
		lua_getglobal(L, "Ent");
		lua_getfield(L, -1, "ptr");
		Entity* pEnt = (Entity*)lua_touserdata(L, -1);
		pEnt->SetScale(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
		return 0;
	}

	int Entity::Lua_Move(lua_State* L)
	{
		lua_getglobal(L, "Ent");
		lua_getfield(L, -1, "ptr");
		Entity* pEnt = (Entity*)lua_touserdata(L, -1);
		pEnt->Move(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
		return 0;
	}

	int Entity::Lua_Rotate(lua_State* L)
	{
		return 0;
	}

	int Entity::Lua_Scale(lua_State* L)
	{
		return 0;
	}

#define COMPDUPL(type, en) \
if(otherEnt.m_hasComponents & en) \
{ \
	type* m = AddComponent<type>(); \
	m->DuplicateFrom(otherEnt.GetComponent<type>()); \
} \

	void Entity::DuplicateCompDataFrom(Entity& otherEnt)
	{
		//m_hasComponents = otherEnt.m_hasComponents;
		//if (m_hasComponents & PrimtTech::ec_meshRef)
		//{
		//	pt::MeshRef* m = AddComponent<MeshRef>();
		//	m->DuplicateFrom(otherEnt.GetComponent<MeshRef>());
		//}
		//if (otherEnt.m_hasComponents & PrimtTech::ec_transform) {
		//	TransformComp* m = AddComponent<TransformComp>();
		//	m->DuplicateFrom(otherEnt.GetComponent<TransformComp>());
		//}
		COMPDUPL(TransformComp, PrimtTech::ec_transform);
		COMPDUPL(MeshRef, PrimtTech::ec_meshRef);
		COMPDUPL(Camera, PrimtTech::ec_cam);
		COMPDUPL(Light, PrimtTech::ec_light);
		COMPDUPL(LuaScript, PrimtTech::ec_lua);
		COMPDUPL(PhysicsBody, PrimtTech::ec_rigidBodies);
	}

	bool pt::Entity::HasComponentType(uint comp) const
	{
		return (m_hasComponents & comp);
	}

	int pt::Entity::CalculateNrOfComponents() const
	{
		int num = 0;
		for (int i = 0; i < NUM_COMPONENT_TYPES; i++)
			num += (int)HasComponentType(1 << i);

		return num;
	}

	void Entity::InsertTable(PrimtTech::HasComponent key, uint val)
	{
		m_hasComponents |= key;
		m_compTable.emplace(key, val);
	}

	void Entity::SetName(const std::string& name)
	{
		if (name != "")
		{
			m_displayName = name;
		}
	}

	template <class T>
	T* addComp(lua_State* L, std::string TypeName, pt::Entity* pEnt)
	{
		const char* name = lua_tostring(L, 2);

		T* pImag = pEnt->AddComponent<T>();
		if (name)
			pImag->Init(name);

		lua_pushlightuserdata(L, pImag);

		TypeName += "MetaTable";

		luaL_getmetatable(L, TypeName.c_str());
		assert(lua_istable(L, -1));
		lua_setmetatable(L, -2);

		return pImag;
	}

	int Entity::Lua_AddComp(lua_State* L)
	{
		lua_getglobal(L, "Ent");
		lua_getfield(L, -1, "ptr");
		//Lua_dumpStack(L);

		int type = lua_tointeger(L, 1);

		pt::Entity* e = (pt::Entity*)lua_touserdata(L, -1);

		switch (type)
		{
		/*//case PrimtTech::ec_transform:
		//{
		//	lua_pushlightuserdata(L, &pEnt->Transform());

		//	int d = luaL_getmetatable(L, "TransformMetaTable");
		//	assert(lua_istable(L, -1));
		//	lua_setmetatable(L, -2);

		//	break;
		//}*/
		case PrimtTech::ec_meshRef:
		{
			// args (string, int)
			pt::MeshRef* mr = addComp<MeshRef>(L, "MeshRef", e);
			const char* name = lua_tostring(L, 2);

			//pt::MeshRef* pImag = pEnt->AddComponent<pt::MeshRef>();
			if (name)
				mr->Init(name);

			//lua_pushlightuserdata(L, pImag);

			//luaL_getmetatable(L, "MeshRefMetaTable");
			//assert(lua_istable(L, -1));
			//lua_setmetatable(L, -2);
			break;
		}
		case PrimtTech::ec_cam:
		{
			// args (string, int)

			const char* name = lua_tostring(L, 2);

			pt::Camera* pImag = e->AddComponent<pt::Camera>();

			lua_pushlightuserdata(L, pImag);

			luaL_getmetatable(L, "CameraMetaTable");
			assert(lua_istable(L, -1));
			lua_setmetatable(L, -2);
			break;
		}
		case PrimtTech::ec_light:
		{
			// args (string, int)

			const char* name = lua_tostring(L, 2);

			pt::Light* pImag = e->AddComponent<pt::Light>();

			lua_pushlightuserdata(L, pImag);

			luaL_getmetatable(L, "LightMetaTable");
			assert(lua_istable(L, -1));
			lua_setmetatable(L, -2);
			break;
		}
		default:
			break;
		}

		return 1;
	}

	int Entity::Lua_GetComp(lua_State* L)
	{
		lua_getglobal(L, "Ent");
		lua_getfield(L, -1, "ptr");

		int type = lua_tointeger(L, 1);

		pt::Entity* pEnt = (pt::Entity*)lua_touserdata(L, -1);

		switch (type)
		{
		case PrimtTech::ec_transform:
		{
			lua_pushlightuserdata(L, &pEnt->Transform());

			luaL_getmetatable(L, "TransformMetaTable");
			assert(lua_istable(L, -1));
			lua_setmetatable(L, -2);
			break;
		}
		case PrimtTech::ec_meshRef:
		{
			THROW_POPUP_ERROR(false, "Not implemented yet");

			break;
		}
		case PrimtTech::ec_cam:
		{
			THROW_POPUP_ERROR(false, "Not implemented yet");

			break;
		}
		case PrimtTech::ec_light:
		{
			THROW_POPUP_ERROR(false, "Not implemented yet");

			break;
		}
		case PrimtTech::ec_rigidBodies:
		{
			THROW_POPUP_ERROR(false, "Not implemented yet");
			break;
		}
		default:
			THROW_POPUP_ERROR(false, "Not implemented yet");
			break;
		}
		return 1;
	}

	int Entity::Lua_FreeComp(lua_State* L)
	{
		return 1;
	}
	uint Entity::GetNoUsedEnts()
	{
		return nrOfEntitiesUsed;
	}
	void Entity::SetNoUsedEnts(EntIdType n)
	{
		nrOfEntitiesUsed = n;
	}
	void Entity::SetNoEnts(int n)
	{
		nrOfEntities = n;
	}
}


