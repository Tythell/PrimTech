#pragma once
#include "Math/Math.h"
#include"../Buffer.h"
#include"Vertex.h"
#include "Material.h"
#include "LoaderFuncs.h"
namespace PrimtTech
{
	/// <summary>
	/// Asset, always created by the ResourceHandler
	/// </summary>
	

	struct MeshInstance
	{
		sm::Vector3 row = sm::Vector3(1.f,0.f,0.f);
		sm::Vector3 row1 = sm::Vector3(0.f,1.f,0.f);
		sm::Vector3 row2 = sm::Vector3(0.f,0.f,1.f);
		sm::Vector3 row3 = sm::Vector3(0.f,0.f,0.f);
	};

	class Mesh
	{
	public:
		Mesh(std::string path, ID3D11Device*& device, ID3D11DeviceContext*& deviceContext, bool makeLeftHanded = true);
		Buffer<Vertex3D>& GetVBuffer();
		std::string GetName() const;
		const uint GetNofMeshes() const;
		std::vector<Mtl> GetMtl() const;
		std::vector<int> GetMtlIndex() const;
		std::vector<int> GetMeshOffsfets() const;

		void InitInstanceBuffer(uint numInstances, ID3D11Device*& d, ID3D11DeviceContext*& dc);
		void Bind(ID3D11DeviceContext*& dc);
		void InstancedBind(ID3D11DeviceContext*& dc);
		void ChangeInstance(uint i, MeshInstance instance);
		void MapInstance();

		int IncreaseUses(int n);
		int GetNrOfUses() const;
		int GetNrOfMaxInstances() const;

		void Release();
	private:
		Buffer<Vertex3D> m_vbuffer;
		MeshInstance* m_pmeshInstArr = nullptr;
		Buffer<MeshInstance> m_instancebuffer;

		uint m_numInstances = 1;
		int m_numActiveInstances = 0;
		uint offset[2] = { 0,0 };

		//uint m_meshIndex = 0, m_noVerts = 0;

		std::vector<Mtl> m_mtls;
		std::vector<int> m_mtlIndexes;
		std::vector<int> m_offsets;
		std::string m_name = "unloaded";
		uint m_nofMeshes = 0;
		Shape m_shape;
	};
}

