#include"pch.h"
#include "ResourceHandler.h"
#include "Mesh.h"

namespace PrimtTech
{
	std::vector<Prefab> ResourceHandler::m_prefabs;
	std::vector<Mesh> ResourceHandler::m_meshes;
	std::vector<TextureMap*> ResourceHandler::m_textures;
	std::vector<pt::Material> ResourceHandler::m_materials;
	ID3D11Device* ResourceHandler::pDevice = nullptr;
	ID3D11DeviceContext* ResourceHandler::s_pDc = nullptr;

	void ResourceHandler::SetDevice(ID3D11Device*& device, ID3D11DeviceContext*& dc)
	{
		pDevice = device;
		s_pDc = dc;
	}

	void ResourceHandler::LoadPak(std::string path)
	{
		THROW_POPUP_ERROR(false, "TODO: FIX PAK-LOADER");
	}

	void ResourceHandler::ReserveMeshMemory(int num)
	{
		m_meshes.reserve(num);
	}

	void ResourceHandler::ReserveTextureMemory(int num)
	{
		m_textures.reserve(num);
	}

	void ResourceHandler::ReserveMaterialMemory(int num)
	{
		m_materials.reserve(num);
	}

	Mesh* ResourceHandler::AddMesh(std::string path, bool makeLeftHanded)
	{
		//THROW_POPUP_ERROR(!(m_meshes.size() == m_meshes.capacity()), "not enough memory reserved for new mesh");
		//uint reserves = m_meshes.capacity();
		for (int i = 0; i < m_meshes.size(); i++)
		{
			if (m_meshes[i].GetName() == StringHelper::GetName(path))
				return &m_meshes[i];
		}



		return &m_meshes.emplace_back(path, pDevice, s_pDc, makeLeftHanded);
	}

	Mesh& ResourceHandler::GetMesh(unsigned int index)
	{
		return m_meshes[index];
	}

	Mesh* ResourceHandler::GetMeshAdress(unsigned int index)
	{
		return &m_meshes[index];
	}

	TextureMap* ResourceHandler::AddDynamicCPUTexture(std::string path)
	{
		TextureMap* pTexture = new TextureMap;
		if (!pTexture->CreateDynamicTexture(path.c_str(), pDevice, s_pDc))
		{
			delete pTexture;
			return m_textures[0];
		}
		m_textures.emplace_back(pTexture);
		return pTexture;
	}

	TextureMap* ResourceHandler::AddTexture(std::string path, uchar flags)
	{
		TextureMap* pTexture = new TextureMap(/*path.c_str(), pDevice, flipUV*/);

		if (flags &= TextureMap::Flags::eDynamic)
		{
			int2 dimensions(0);
			unsigned char* image = nullptr;
			//path = "Assets/Textures/" + path;
			FileLoader::StbiCreateCharFromFile(path.c_str(), image, dimensions.x, dimensions.y, 4);
			pTexture->CreateDynamicTexture(image, dimensions, pDevice, s_pDc);
			pTexture->SetName(StringHelper::GetName(path));
		}
		else
		{
			if (!pTexture->CreateFromFile(path.c_str(), pDevice, DX11))
			{
				delete pTexture;
				return m_textures[0];
			}
			//m_textures.emplace_back(pTexture);
		}

		m_textures.emplace_back(pTexture);
		return pTexture;
	}

	TextureMap& ResourceHandler::GetTexture(unsigned int index)
	{
		return *m_textures[index];
	}

	TextureMap* ResourceHandler::GetTextureAdress(unsigned int index)
	{
		return m_textures[index];;
	}

	TextureMap* ResourceHandler::ReloadTexture(std::string name, std::string newPath)
	{
		int texIndex = CheckTextureNameExists(name);

		if (texIndex != -1)
			return m_textures[texIndex]->CreateFromFile(newPath.c_str(), pDevice) ? m_textures[texIndex] : nullptr;

		return nullptr;
	}

	const uint ResourceHandler::GetNumTextures()
	{
		return static_cast<uint>(m_textures.size());
	}

	pt::Material* ResourceHandler::AddMaterial(std::string name)
	{
		//THROW_POPUP_ERROR(!(m_materials.size() == m_materials.capacity()), "not enough memory reserved for new material");

		//for (int i = 0; i < m_materials.size(); i++)
		//{
		//	if (m_materials[i].GetFileName() == StringHelper::GetName(name))
		//		return &m_materials[i];
		//}

		return &m_materials.emplace_back(name);
	}

	pt::Material& ResourceHandler::GetMaterial(unsigned int index)
	{
		return m_materials[index];
	}

	pt::Material* ResourceHandler::GetMaterialAdress(unsigned int index)
	{
		return &m_materials[index];
	}

	int ResourceHandler::CheckMtrlNameExists(std::string mtrlName)
	{
		for (int i = 0; i < m_materials.size(); i++)
		{
			if (m_materials[i].GetFileName() == mtrlName)
				return i;
		}
		return -1;
	}

	uint ResourceHandler::GetNoMaterials()
	{
		return (uint)m_materials.size();
	}

	uint ResourceHandler::GetNoMeshes()
	{
		return (uint)m_meshes.size();
	}

	const uint ResourceHandler::GetMtrlCount()
	{
		return (uint)m_materials.size();
	}

	int ResourceHandler::CheckMeshNameExists(std::string meshName)
	{
		for (int i = 0; i < m_meshes.size(); i++)
		{
			if (m_meshes[i].GetName() == meshName)
				return i;
		}
		return -1;
	}

	int ResourceHandler::CheckTextureNameExists(std::string textureName)
	{
		for (int i = 0; i < m_textures.size(); i++)
		{
			if (m_textures[i]->GetName() == textureName)
				return i;
		}
		return -1;
	}

	void ResourceHandler::Unload()
	{
		for (int i = 0; i < m_textures.size(); i++)
			delete m_textures[i];
		for (int i = 0; i < m_meshes.size(); i++)
			m_meshes[i].Release();
		for (int i = 0; i < m_prefabs.size(); i++)
			m_prefabs[i].Release();
	}

	ID3D11Device* ResourceHandler::GetDevice()
	{
		return pDevice;
	}

	Prefab& ResourceHandler::NewPrefab(const std::string& name, uint maxCap)
	{
		return m_prefabs.emplace_back(pDevice, s_pDc, maxCap, name);
	}

	Prefab& ResourceHandler::GetPrefab(uint index)
	{
		return m_prefabs[index];
	}

	std::vector<Prefab>& ResourceHandler::GetPrefabArray()
	{
		return m_prefabs;
	}

	void ResourceHandler::ResizeMaterials(uint u)
	{
		m_materials.resize(u);
	}

	void ResourceHandler::InitInstancesForMesh(uint i, uint slots)
	{
		m_meshes[i].InitInstanceBuffer(slots, pDevice, s_pDc);
	}
	//MeshCluster::MeshCluster()
	//{
	//	//m_startIndexes.emplace_back(0);
	//}
	//int MeshCluster::AddMesh(std::vector<Vertex3D>& vec, std::string name)
	//{
	//	//int meshExists = -1;

	//	//for (int i = 0;/* meshExists == -1 &&*/ i < m_names.size(); i++)
	//	//{
	//	//	if (m_names[i] == name)
	//	//	{
	//	//		//meshExists = i;
	//	//		return i;
	//	//	}
	//	//}

	//	//int returnIndex = (int)m_allVerts.size() - 1;

	//	//m_allVerts.reserve(m_allVerts.size() + vec.size());
	//	//m_allVerts.insert(m_allVerts.end(), vec.begin(), vec.end());

	Prefab::Prefab(ID3D11Device*& d, ID3D11DeviceContext*& dc, uint maxCap, std::string name):
		m_prefabName(name), m_maxNoInstances(maxCap)
	{
		InitInstanceBuffer(maxCap, d, dc);
		m_materialIndex.resize(1,0);
	}

	void Prefab::SetMaterial(uint slot, std::string material)
	{
		SetMaterial(slot, std::max(ResourceHandler::CheckMtrlNameExists(material), 0));
	}

	void Prefab::SetMaterial(uint slot, uint materialIndex)
	{
		m_materialIndex[slot] = std::max(0u, materialIndex);
	}

	void Prefab::SetMesh(uint meshIndex)
	{
		m_meshIndex = meshIndex;
		m_materialIndex.resize(ResourceHandler::GetMesh(m_meshIndex).GetNofMeshes(),0);
	}

	void Prefab::SetMesh(std::string meshName)
	{
		SetMesh(std::max(0, ResourceHandler::CheckMeshNameExists(meshName)));
	}

	void Prefab::InitInstanceBuffer(uint numInstances, ID3D11Device*& d, ID3D11DeviceContext*& dc)
	{
		if (m_pmeshInstArr)
			delete[] m_pmeshInstArr;
		m_pmeshInstArr = new matrix[numInstances];

		m_maxNoInstances = numInstances;

		m_instancebuffer.CreateVertexBuffer(d, m_pmeshInstArr, numInstances, dc, eBufferFlags_IgnoreCreateTwice);
	}

	void Prefab::BindInstanceBuffer(ID3D11DeviceContext*& dc)
	{
		//MeshInstance debugArr[4];
		//memcpy(debugArr, m_pmeshInstArr, sizeof(MeshInstance) * 4);

		uint offset[] = { 0 };
		dc->IASetVertexBuffers(1, 1, m_instancebuffer.GetReference(), m_instancebuffer.GetStrideP(), offset);
	}

	void Prefab::ChangeInstance(uint i, const matrix& inst)
	{
		m_instancebuffer.Data(i) = inst;
	}

	void Prefab::MapInstBuffer()
	{
		m_instancebuffer.MapBuffer();
	}

	void Prefab::SetName(std::string meshName)
	{
		m_prefabName = meshName;
	}

	std::string Prefab::GetName() const
	{
		return m_prefabName;
	}

	std::string Prefab::GetMeshName() const
	{
		return ResourceHandler::GetMesh(m_meshIndex).GetName();
	}

	uint Prefab::GetMeshIndex() const
	{
		return m_meshIndex;
	}

	uint Prefab::GetMaterialIndex(uint i) const
	{
		return m_materialIndex[i];
	}

	Mesh* Prefab::GetMeshPtr() const
	{
		return ResourceHandler::GetMeshAdress(m_meshIndex);
	}

	void Prefab::AddRefIdx(uint idx)
	{
		m_prefabRefsIdx.emplace_back(idx);
	}

	uint Prefab::GetRefIdx(uint idx) const
	{
		return m_prefabRefsIdx[idx];
	}

	uint Prefab::GetNOfRefIdx() const
	{
		return (uint)m_prefabRefsIdx.size();
	}

	uint Prefab::IncreaseUses(int num)
	{
		m_noOfUses += num;
		return (uint)std::max(0, m_noOfUses);
	}

	uint Prefab::GetUses() const
	{
		return m_noOfUses;
	}

	uint Prefab::GetCap() const
	{
		return m_maxNoInstances;
	}

	void Prefab::Release()
	{
		delete[] m_pmeshInstArr;

		m_instancebuffer.Release();
	}

	//	//return returnIndex;
	//	return 0;
	//}
	//void MeshCluster::IASet(ID3D11DeviceContext* dc)
	//{
	//	uint offset[] = { 0 };
	//	//dc->IASetVertexBuffers(0, 1, m_mainVbuffer.GetReference(), m_mainVbuffer.GetStrideP(), offset);
	//}
}


