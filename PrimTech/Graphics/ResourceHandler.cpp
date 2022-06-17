#include "ResourceHandler.h"
#include "Model.h"

std::vector<Mesh*> ResourceHandler::m_meshes;

Mesh* ResourceHandler::AddMesh(std::string path, ID3D11Device*& pDevice, bool makeLeftHanded)
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

int ResourceHandler::CheckMeshNameExists(std::string meshName)
{
	for (int i = 0; i < m_meshes.size(); i++)
	{
		if (m_meshes[i]->GetName() == meshName)
			return i;
	}
	return -1;
}

void ResourceHandler::Unload()
{
	for (int i = 0; i < m_meshes.size(); i++)
	{
		delete m_meshes[i];
	}
}