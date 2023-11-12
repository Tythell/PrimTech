#pragma once
#include "Component.h"

namespace pt
{

	class TransformComp;

	class MeshRef : public Component
	{
	public:
		MeshRef(EntIdType entId, std::string meshName = "");

		void SetMesh(const std::string& path);
		void SetIndex(uint idx);
		void SetMaterial(uint materialIndex, uint slot = 0);
		void SetMaterial(const std::string& materialName, uint slot = 0);

		void UpdateTransformInstance(PrimtTech::Mesh* meshArr, const TransformComp& t);

		PrimtTech::Mesh* GetMeshContainerP() const { return PrimtTech::ResourceHandler::GetMeshAdress(m_meshIndex); }
		uint GetMaterialIndex(const uint& index) const;
		uint GetNumMaterials() const { return (uint)m_pMaterialindexes.size(); };
		std::string GetNameOfMesh() const { return PrimtTech::ResourceHandler::GetMeshAdress(m_meshIndex)->GetName(); }
		uint GetInstIndex() const;

		static int Lua_ChangeModel(lua_State* L);
		// Inherited via Component
		virtual void DuplicateFrom(Component* other) override;
		virtual void OnFree() override;

		enum Type
		{
			eRegularMesh, ePrefab
		};
		void SetType(Type type);
		Type GetType() const;
		uint GetIndex() const;
	private:
		Type m_type = eRegularMesh;
		uint m_meshIndex = 0;
		int m_instanceIndex = 0;
		std::vector<uint> m_pMaterialindexes;
	};

	class MeshPrefabRef : public Component
	{
	public:
		MeshPrefabRef(EntIdType entId);

		void SetPrefabIndex(uint i);
		//void SetPrefab(std::string i);


		static int Lua_ChangePrefab(lua_State* L);
		// Inherited via Component
		virtual void DuplicateFrom(Component* other) override;
		virtual void OnFree() override;
		void RefreshInstance();

		uint GetIndex() const;
		uint GetInstIndex() const;
	private:
		//Type m_type = eRegularMesh;
		uint m_prefabIndex = 0;
		uint m_instanceIndex = 0;
	};
}
