#pragma once
#include <d3d11.h>
#include "../Buffer.h"
#include "Vertex.h"
#include <string>
#include <vector>

namespace pt
{
	class Material;
}

namespace PrimtTech
{
	class Mesh;
	class MeshInstance;
	class TextureMap;


	class Prefab
	{
	public:
		Prefab(ID3D11Device *& d, ID3D11DeviceContext*& dc, uint maxCap, std::string name = "");

		void SetMaterial(uint slot, std::string material);
		void SetMaterial(uint slot, uint materialIndex);

		void SetMesh(uint meshIndex);
		void SetMesh(std::string meshName);

		void InitInstanceBuffer(uint numInstances, ID3D11Device*& d, ID3D11DeviceContext*& dc);
		void BindInstanceBuffer(ID3D11DeviceContext*& dc);
		void ChangeInstance(uint i, const matrix& inst);
		void MapInstBuffer();

		void SetName(std::string name);
		std::string GetName() const;
		std::string GetMeshName() const;
		uint GetMeshIndex() const;
		uint GetMaterialIndex(uint i) const;
		Mesh* GetMeshPtr() const;

		void AddRefIdx(uint idx);
		uint GetRefIdx(uint idx) const;
		uint GetNOfRefIdx() const;
		uint IncreaseUses(int num);
		uint GetUses() const;
		uint GetCap() const;

		void Release();
	private:
		matrix* m_pmeshInstArr = nullptr;
		Buffer<matrix> m_instancebuffer;
		std::vector<uint> m_prefabRefsIdx;

		uint m_meshIndex = 0;
		int m_noOfUses = 0;
		int m_maxNoInstances = 0;
		std::vector<uint> m_materialIndex;
		std::string m_prefabName = "";
	};

	class ResourceHandler
	{
	public:
		static void SetDevice(ID3D11Device*& device, ID3D11DeviceContext*& dc);
		static void LoadPak(std::string path);
		//static Mesh* AddMesh(std::string path, bool makeLeftHanded = true);
		static void ReserveMeshMemory(int num);
		static void ReserveTextureMemory(int num);
		static void ReserveMaterialMemory(int num);
		static Mesh* AddMesh(std::string path, bool makeLeftHanded = true);
		static Mesh& GetMesh(unsigned int index);
		static Mesh* GetMeshAdress(unsigned int index);
		static std::vector<Mesh>& GetMeshArrayReference() { return m_meshes; };
		static std::vector<pt::Material>& GetMaterialArrayReference() { return m_materials; };
		static std::vector<TextureMap*>& GetTextureArrayReference() { return m_textures; };
		static TextureMap* AddDynamicCPUTexture(std::string path);
		static TextureMap* AddTexture(std::string path, uchar flags = 0);
		static TextureMap& GetTexture(unsigned int index);
		static TextureMap* GetTextureAdress(unsigned int index);
		static TextureMap* ReloadTexture(std::string path, std::string newPath);
		static const uint GetNumTextures();
		static pt::Material* AddMaterial(std::string name);
		static pt::Material& GetMaterial(unsigned int index);
		static pt::Material* GetMaterialAdress(unsigned int index);
		static int CheckMtrlNameExists(std::string mtrlName);
		static uint GetNoMaterials();
		static uint GetNoMeshes();
		static const uint GetMtrlCount();
		static int CheckMeshNameExists(std::string meshName);
		static int CheckTextureNameExists(std::string textureName);
		static void Unload();
		static ID3D11Device* GetDevice();

		static Prefab& NewPrefab(const std::string& name = "", uint maxCap = 4);
		static Prefab& GetPrefab(uint index);
		static std::vector<Prefab>& GetPrefabArray();

		static void ResizeMaterials(uint u);
		static void InitInstancesForMesh(uint i, uint slots);
	private:
		static std::vector<Prefab> m_prefabs;
		static std::vector<Mesh> m_meshes;
		static std::vector<TextureMap*> m_textures;
		static std::vector<pt::Material> m_materials;
		static ID3D11Device* pDevice;
		static ID3D11DeviceContext* s_pDc;
	};
}

