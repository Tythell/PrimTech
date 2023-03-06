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
}


