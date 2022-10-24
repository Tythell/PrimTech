#include "ResourceHandler.h"
#include "Model.h"

std::vector<Mesh*> ResourceHandler::m_meshes;
std::vector<TextureMap*> ResourceHandler::m_textures;
std::vector<Material*> ResourceHandler::m_materials;
ID3D11Device* ResourceHandler::pDevice;

void ResourceHandler::SetDevice(ID3D11Device*& device)
{
	pDevice = device;
}

Mesh* ResourceHandler::AddMesh(std::string path, unsigned char makeLeftHanded)
{
	Mesh* pMesh = new Mesh(path, pDevice, makeLeftHanded);
	m_meshes.emplace_back(pMesh);
	return pMesh;
}

Mesh& ResourceHandler::GetMesh(unsigned int index)
{
	return *m_meshes[index];
}

Mesh* ResourceHandler::GetMeshAdress(unsigned int index)
{
	return m_meshes[index];
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

Material* ResourceHandler::AddMaterial(std::string name, bool nameNewIsNew)
{
	for (int i = 0; i < m_materials.size(); i++)
	{
		if (m_materials[i]->GetFileName() == name && nameNewIsNew)
		{
			name.append("1");
			return AddMaterial(name, true);
		}
		else if (m_materials[i]->GetFileName() == name)
			return m_materials[i];
	}
	Material* pMat = new Material(name, MaterialType::Resource);
	m_materials.emplace_back(pMat);
	return pMat;
}

void ResourceHandler::DeleteMaterial(std::string name)
{
	for (int i = 0; i < m_materials.size(); i++)
	{
		if (m_materials[i]->GetFileName() == name)
		{
			delete m_materials[i];
			m_materials.erase(m_materials.begin() + i);
		}
	}
}

Material* ResourceHandler::GetMaterial(std::string name)
{
	for (int i = 0; i < m_materials.size(); i++)
	{
		if (m_materials[i]->GetFileName() == name)
		{
			return m_materials[i];
		}
	}
	return nullptr;
}

Material* ResourceHandler::GetMaterial(unsigned int index)
{
	return m_materials[index];
}

uint ResourceHandler::GetMtrlAmount()
{
	return m_materials.size();
}

int ResourceHandler::CheckMeshNameExists(std::string meshName)
{
	for (int i = 0; i < m_meshes.size(); i++)
	{
		if (m_meshes[i]->GetName() == meshName)
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
	for (int i = 0; i < m_meshes.size(); i++)
		delete m_meshes[i];
	for (int i = 0; i < m_textures.size(); i++)
		delete m_textures[i];
	for (int i = 0; i < m_materials.size(); i++)
		delete m_materials[i];
}

ID3D11Device* ResourceHandler::GetDevice()
{
	return pDevice;
}

void ResourceHandler::ResetUses()
{
	for (int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i]->ResetUses();
	}
}
