#pragma once
#include <d3d11.h>
#include <string>
#include <vector>

class Mesh;

class ResourceHandler
{
public:
	static Mesh* AddMesh(std::string path, ID3D11Device*& pDevice, bool makeLeftHanded = true);
	static Mesh& GetMesh(unsigned int index);
	static Mesh* GetMeshAdress(unsigned int index);
	static int CheckMeshNameExists(std::string meshName);
	static void Unload();
private:
	static std::vector<Mesh*> m_meshes;

};