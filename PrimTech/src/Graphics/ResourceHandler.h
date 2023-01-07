#pragma once
#include <d3d11.h>
#include "../Math/Math.h"
#include <string>
#include <vector>

class Mesh;
class TextureMap;
class Material;

class ResourceHandler
{
public:
	static void SetDevice(ID3D11Device*& device);
	//static Mesh* AddMesh(std::string path, bool makeLeftHanded = true);
	static Mesh* AddMesh(std::string path, bool makeLeftHanded = true);
	static Mesh& GetMesh(unsigned int index);
	static Mesh* GetMeshAdress(unsigned int index);
	static TextureMap* AddTexture(std::string path, bool flipUV = true);
	static TextureMap& GetTexture(unsigned int index);
	static TextureMap* GetTextureAdress(unsigned int index);
	static Material* AddMaterial(std::string name);
	static Material& GetMaterial(unsigned int index);
	static Material* GetMaterialAdress(unsigned int index);
	static const uint GetMtrlCount();
	static int CheckMeshNameExists(std::string meshName);
	static int CheckTextureNameExists(std::string textureName);
	static void Unload();
	static ID3D11Device* GetDevice();
	static void ResetUses();
private:
	static std::vector<Mesh*> m_meshes;
	static std::vector<TextureMap*> m_textures;
	static std::vector<Material*> m_materials;
	static ID3D11Device* pDevice;
};