#pragma once
#include <d3d11.h>
#include <string>
#include <vector>

class Mesh;
class TextureMap;

class ResourceHandler
{
public:
	static void SetDevice(ID3D11Device*& device);
	static Mesh* AddMesh(std::string path, bool makeLeftHanded = true);
	static Mesh& GetMesh(unsigned int index);
	static Mesh* GetMeshAdress(unsigned int index);
	static TextureMap* AddTexture(std::string path, bool flipUV = true);
	static TextureMap& GetTexture(unsigned int index);
	static TextureMap* GetTextureAdress(unsigned int index);
	static int CheckMeshNameExists(std::string meshName);
	static int CheckTextureNameExists(std::string textureName);
	static void Unload();
private:
	static std::vector<Mesh*> m_meshes;
	static std::vector<TextureMap*> m_textures;
	static ID3D11Device* pDevice;
};