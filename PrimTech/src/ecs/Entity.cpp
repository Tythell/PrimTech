#include "pch.h"
#include "Entity.h"

namespace pt
{
	uint Entity::nrOfEntities = 0;

	std::vector<Entity> Entity::s_ents;

	Entity& Entity::Create()
	{
		return s_ents.emplace_back(true);
	}

	const uint Entity::NumEnts()
	{
		return (uint)s_ents.size();
	}

	Entity& Entity::GetEntity(uint index)
	{
		return s_ents[index];
	}
	Entity* Entity::GetEntityP(uint index)
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

	bool pt::Entity::HasComponentType(uint comp) const
	{
		if (m_hasComponents & comp)
			return true;
		return false;
	}
	void Entity::InsertTable(uint key, uint val)
	{
		m_hasComponents &= key;
		m_compTable.emplace(key, val);
	}
}


