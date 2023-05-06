#include"pch.h"
#include "ResourceHandler.h"
#include "Mesh.h"

namespace PrimtTech
{
	std::vector<Mesh> ResourceHandler::m_meshes;
	std::vector<TextureMap*> ResourceHandler::m_textures;
	std::vector<Material> ResourceHandler::m_materials;
	ID3D11Device* ResourceHandler::pDevice;
	ID3D11DeviceContext* ResourceHandler::s_pDc;

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

		return &m_meshes.emplace_back(path, pDevice, makeLeftHanded);
	}

	Mesh& ResourceHandler::GetMesh(unsigned int index)
	{
		return m_meshes[index];
	}

	Mesh* ResourceHandler::GetMeshAdress(unsigned int index)
	{
		return &m_meshes[index];
	}

	TextureMap* ResourceHandler::AddTexture(std::string path, bool flipUV)
	{
		TextureMap* pTexture = new TextureMap(/*path.c_str(), pDevice, flipUV*/);
		if (!pTexture->CreateFromFile(path.c_str(), pDevice, flipUV))
		{
			delete pTexture;
			return m_textures[0];
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

	Material* ResourceHandler::AddMaterial(std::string name)
	{
		//THROW_POPUP_ERROR(!(m_materials.size() == m_materials.capacity()), "not enough memory reserved for new material");

		//for (int i = 0; i < m_materials.size(); i++)
		//{
		//	if (m_materials[i].GetFileName() == StringHelper::GetName(name))
		//		return &m_materials[i];
		//}

		return &m_materials.emplace_back(name);
	}

	Material& ResourceHandler::GetMaterial(unsigned int index)
	{
		return m_materials[index];
	}

	Material* ResourceHandler::GetMaterialAdress(unsigned int index)
	{
		return &m_materials[index];
	}

	int ResourceHandler::CheckMtrlNameExists(std::string mtrlName)
	{
		for (int i = 0; i < m_meshes.size(); i++)
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
		return m_meshes.size();
	}

	const uint ResourceHandler::GetMtrlCount()
	{
		return m_materials.size();
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
	}

	ID3D11Device* ResourceHandler::GetDevice()
	{
		return pDevice;
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

	//	//return returnIndex;
	//	return 0;
	//}
	//void MeshCluster::IASet(ID3D11DeviceContext* dc)
	//{
	//	uint offset[] = { 0 };
	//	//dc->IASetVertexBuffers(0, 1, m_mainVbuffer.GetReference(), m_mainVbuffer.GetStrideP(), offset);
	//}
}


