#pragma once
#include "ComponentHandler.h"
#include<map>

namespace pt
{
	class Entity
	{
	public:
		static std::vector<Entity*> s_ents;

		Entity(unsigned char flags = 0)
		{
			m_id = nrOfEntities++;



			s_ents.emplace_back(this);

			AddComponent<pt::TransformComp>();
		}

		template<class T>
		T* AddComponent()
		{
			PrimtTech::HasComponent c = PrimtTech::ec_null;
			std::vector<T>* ptvec = nullptr;

			PrimtTech::ComponentHandler::LinkVector<T>(ptvec, c);

			ptvec->emplace_back(m_id);

			if (m_hasComponents & c)
				return &ptvec[0][m_compTable[c]];

			m_compTable.emplace(c, (uint)ptvec->size() - 1);

			m_hasComponents |= (uint)c;
			return &ptvec[0][(uint)ptvec->size() - 1];

			return NULL;
		}

		template<class T>
		T* GetComponent()
		{
			std::vector<T>* ptvec = nullptr;
			PrimtTech::HasComponent c = PrimtTech::ec_null;

			PrimtTech::ComponentHandler::LinkVector<T>(ptvec, c);

			if (m_hasComponents & c)
				return &ptvec[0][m_compTable[c]];

			return NULL;
		}
		template<class T>
		uint GetComponentIndex()
		{

			std::vector<T>* dummy = nullptr;
			PrimtTech::HasComponent c = PrimtTech::ec_null;

			PrimtTech::ComponentHandler::LinkVector<T>(dummy, c);

			return m_compTable[c];
		}

		pt::TransformComp& Transform()
		{
			return PrimtTech::ComponentHandler::GetComponentArray<pt::TransformComp>()[m_id];
		}

		bool HasComponentType(uint comp) const;

		int CalculateNrOfComponents() const
		{
			int num = 0;
			for (int i = 0; i < NUM_COMPONENT_TYPES; i++)
			{
				if (HasComponentType(1 << i))
				{
					num++;
				}
			}
			return num;
		};
		std::map<uint, uint> GetCompTable() { return m_compTable; };

		void InsertTable(uint key, uint val);
	private:
		uint m_id = 0xffffffff;
		uint m_hasComponents = 0;

		static uint nrOfEntities;
		std::/*unordered_*/map<uint, uint> m_compTable;
	};
}

