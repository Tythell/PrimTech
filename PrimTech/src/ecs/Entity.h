#pragma once
#include "ComponentHandler.h"
#include<map>

namespace pt
{
	class Entity
	{
	public:
		static std::vector<Entity> s_ents;

		Entity(bool createdWithFunc = false)
		{
			m_id = nrOfEntities++;
			THROW_POPUP_ERROR(createdWithFunc, "MANNEN ANVÄND pt::Entity::Create() ISTÄLLET");
			AddComponent<pt::TransformComp>();
			m_displayName = std::to_string(m_id);
		}

		static Entity& Create();
		static const uint NumEnts();
		static Entity& GetEntity(EntIdType index);
		static Entity* GetEntityP(EntIdType index);
		static void Clear(uint until = 0);
		static std::vector<Entity>& GetAllEnts();
		static void ReserveEnts(size_t size);

		void SetPhysBodyIndex(int idx);
		void SetPosition(float x, float y, float z);
		void SetPosition(const sm::Vector3& v);
		void SetRotation(float x, float y, float z);
		void SetRotation(const sm::Vector3& v);
		void SetRotation(const sm::Quaternion& v);
		void SetScale(float x, float y, float z);
		void SetScale(const sm::Vector3& v);

		template<class T>
		T* AddComponent()
		{
			PrimtTech::HasComponent c = PrimtTech::ec_null;
			std::vector<T>* ptvec = nullptr;

			PrimtTech::ComponentHandler::LinkVector<T>(ptvec, c);

			if (m_hasComponents & c)
				return &ptvec[0][m_compTable[c]];

			int freeComponents = PrimtTech::ComponentHandler::GetNoFreeComponents(c);
			int returnIndex = -1;

			if (freeComponents > 0)
			{
				PrimtTech::ComponentHandler::IncreaseFreeComponents(c, -1);
				returnIndex = ptvec->size() - freeComponents;
				ptvec[0][returnIndex] = T(m_id);
			}
			else
			{
				ptvec->emplace_back(m_id);
				returnIndex = (uint)ptvec->size() - 1;
			}

			m_compTable.emplace(c, (uint)returnIndex);

			m_hasComponents |= (uint)c;
			return &ptvec[0][(uint)ptvec->size() - 1u];

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
		void FreeComponent()
		{
			std::vector<T>* ptvec = nullptr;
			PrimtTech::HasComponent c = PrimtTech::ec_null;

			PrimtTech::ComponentHandler::LinkVector<T>(ptvec, c);

			if (m_hasComponents & c)
			{
				int index = m_compTable[c];

				m_hasComponents &= ~c;
				m_compTable.erase(c);

				ptvec[0][index].FreeComponent();

				int swapIndex = ptvec->size() - PrimtTech::ComponentHandler::GetNoFreeComponents(c) - 1;

				std::swap(ptvec[0][index], ptvec[0][swapIndex]);
				PrimtTech::ComponentHandler::IncreaseFreeComponents(c, 1);
			}
		}
		template<class T>
		uint GetComponentIndex()
		{
			std::vector<T>* dummy = nullptr;
			PrimtTech::HasComponent c = PrimtTech::ec_null;

			PrimtTech::ComponentHandler::LinkVector<T>(dummy, c);

			return m_compTable[c];
		}

		pt::TransformComp& Transform() { return PrimtTech::ComponentHandler::GetComponentArray<pt::TransformComp>()[m_id]; }

		bool HasComponentType(uint comp) const;

		int CalculateNrOfComponents() const;
		std::map<uint, uint> GetCompTable() { return m_compTable; };

		void InsertTable(uint key, uint val);

		void SetName(const std::string& name) { m_displayName = name; }
		std::string GetName() const { return m_displayName; };
	private:
		EntIdType m_id = 0xffffffff;
		uint m_hasComponents = 0;
		std::string m_displayName = "";

		int m_physIndex = -1;

		static uint nrOfEntities;
		std::/*unordered_*/map<uint, uint> m_compTable;
	};
}

