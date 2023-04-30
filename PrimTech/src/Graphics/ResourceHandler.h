#pragma once
#include <d3d11.h>
#include "../Math/Math.h"
#include <string>
#include <vector>

namespace PrimtTech
{
	class Mesh;
	class TextureMap;
	class Material;

	class ResourceHandler
	{
	public:
		static void SetDevice(ID3D11Device*& device);
		static void LoadPak(std::string path);
		//static Mesh* AddMesh(std::string path, bool makeLeftHanded = true);
		static void ReserveMeshMemory(int num);
		static void ReserveTextureMemory(int num);
		static void ReserveMaterialMemory(int num);
		static Mesh* AddMesh(std::string path, bool makeLeftHanded = true);
		static Mesh& GetMesh(unsigned int index);
		static Mesh* GetMeshAdress(unsigned int index);
		static std::vector<Mesh>& GetMeshArrayReference() { return m_meshes; };
		static std::vector<Material>& GetMaterialArrayReference() { return m_materials; };
		static std::vector<TextureMap*>& GetTextureArrayReference() { return m_textures; };
		static TextureMap* AddTexture(std::string path, bool flipUV = true);
		static TextureMap& GetTexture(unsigned int index);
		static TextureMap* GetTextureAdress(unsigned int index);
		static Material* AddMaterial(std::string name);
		static Material& GetMaterial(unsigned int index);
		static Material* GetMaterialAdress(unsigned int index);
		static int CheckMtrlNameExists(std::string mtrlName);
		static uint GetNoMaterials();
		static const uint GetMtrlCount();
		static int CheckMeshNameExists(std::string meshName);
		static int CheckTextureNameExists(std::string textureName);
		static void Unload();
		static ID3D11Device* GetDevice();
	private:
		static std::vector<Mesh> m_meshes;
		static std::vector<TextureMap*> m_textures;
		static std::vector<Material> m_materials;
		static ID3D11Device* pDevice;
	};

	class MeshCluster
	{
	public:
		void Init();
		//void AddMesh(Vertex3D* arr, int size);

#ifdef DX11
		void IASet(ID3D11DeviceContext* dc);
#endif // DX11

		
	private:
		std::vector<int> m_allVerts;
		//Buffer<int> m_mainVbuffer;
	};
}

