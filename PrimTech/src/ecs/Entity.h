#pragma once
#include "ComponentHandler.h"
#include<map>

class Entity
{
public:
	static std::vector<Entity*> s_ents;

	Entity(unsigned char flags = 0)
	{
		m_id = nrOfEntities++;



		s_ents.emplace_back(this);

		if (!(flags & 0x01)); // dont include transform
		AddComponent<TransformComp>();
	}

	template<class T>
	T* AddComponent()
	{
		HasComponent c = ec_null;
		std::vector<T>* ptvec = nullptr;

		ComponentHandler::LinkVector<T>(ptvec, c);

		ptvec->emplace_back(m_id);

		if (m_hasComponents & c)
			return &ptvec[0][m_compTable[c]];

		m_compTable.emplace(c, (uint)ptvec->size()-1);

		m_hasComponents |= c;
		return &ptvec[0][(uint)ptvec->size() - 1];

		return NULL;
	}

	template<class T>
	T* GetComponent(const uint& index = 0)
	{
		std::vector<T>* ptvec = nullptr;
		HasComponent c = ec_null;

		ComponentHandler::LinkVector<T>(ptvec, c);

		if (m_hasComponents & c)
			return &ptvec[0][m_compTable[c]];

		return NULL;
	}

	TransformComp& Transform()
	{
		return ComponentHandler::GetComponentArray<TransformComp>()[m_id];
	}

private:
	uint m_id = 0xffffffff;
	uint m_hasComponents = 0;

	static uint nrOfEntities;
	std::/*unordered_*/map<uint, uint> m_compTable;
};