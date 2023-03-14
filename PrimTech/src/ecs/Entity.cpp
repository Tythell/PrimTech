#include "pch.h"
#include "Entity.h"

namespace pt
{
	uint Entity::nrOfEntities = 0;

	std::vector<Entity*> Entity::s_ents;

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


