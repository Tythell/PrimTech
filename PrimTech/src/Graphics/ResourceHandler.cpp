#include "ResourceHandler.h"
#include "Model.h"

std::vector<Mesh> ResourceHandler::m_meshes;
std::vector<TextureMap*> ResourceHandler::m_textures;
std::vector<Material> ResourceHandler::m_materials;
ID3D11Device* ResourceHandler::pDevice;

void ResourceHandler::SetDevice(ID3D11Device*& device)
{
	pDevice = device;
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
	THROW_POPUP_ERROR(!(m_meshes.size() == m_meshes.capacity()), "not enough memory reserved for new mesh");
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
	THROW_POPUP_ERROR(!(m_materials.size() == m_materials.capacity()), "not enough memory reserved for new material");

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
	//for (int i = 0; i < m_meshes.size(); i++)
	//	delete m_meshes[i];
	for (int i = 0; i < m_textures.size(); i++)
		delete m_textures[i];
}

ID3D11Device* ResourceHandler::GetDevice()
{
	return pDevice;
}

void ResourceHandler::ResetUses()
{
	for (int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].ResetUses();
	}
}
