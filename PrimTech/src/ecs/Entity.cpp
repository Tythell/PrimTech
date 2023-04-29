#include "pch.h"
#include "Entity.h"
//#include "ComponentHandler.h"

namespace pt
{
	uint Entity::nrOfEntities = 0;

	std::vector<Entity> Entity::s_ents;

	Entity& Entity::Create()
	{
		return s_ents.emplace_back(true); // Don't question it
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

	void Entity::InsertTable(uint key, uint val)
	{
		m_hasComponents &= key;
		m_compTable.emplace(key, val);
	}
}


