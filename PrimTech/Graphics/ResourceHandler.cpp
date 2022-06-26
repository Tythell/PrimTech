#include "ResourceHandler.h"
#include "Model.h"

std::vector<Mesh*> ResourceHandler::m_meshes;
std::vector<TextureMap*> ResourceHandler::m_textures;
ID3D11Device* ResourceHandler::pDevice;

void ResourceHandler::SetDevice(ID3D11Device*& device)
{
	pDevice = device;
}

Mesh* ResourceHandler::AddMesh(std::string path, bool makeLeftHanded)
{
	Mesh* pMesh = new Mesh(path, pDevice);
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
	TextureMap* pTexture = new TextureMap(path.c_str(), pDevice, flipUV);
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
}