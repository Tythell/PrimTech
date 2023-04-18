#pragma once
#include "Component.h"

namespace pt
{
	class MeshRef : public Component
	{
	public:
		MeshRef(EntIdType entId, std::string meshName = "");

		void Init(std::string path);
		void SetMaterial(uint materialIndex, uint slot = 0);
		void SetMaterial(std::string materialName, uint slot = 0);

		PrimtTech::Mesh* GetMeshContainerP() const { return PrimtTech::ResourceHandler::GetMeshAdress(m_meshIndex); }
		uint GetMaterialIndex(const uint& index) const;
		uint GetNumMaterials() const { return (uint)m_pMaterialindexes.size(); };
		std::string GetNameOfMesh() const { return PrimtTech::ResourceHandler::GetMeshAdress(m_meshIndex)->GetName(); }
	private:
		uint m_meshIndex = 0;
		std::vector<uint> m_pMaterialindexes;
	};
}
