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
	class Mesh
	{
	public:
		Mesh(std::string path, ID3D11Device*& device, bool makeLeftHanded = true);
		//Mesh(const Mesh& other);
		Buffer<Vertex3D>& GetVBuffer();
		std::string GetName() const;
		const uint GetNofMeshes() const;
		std::vector<Mtl> GetMtl() const;
		std::vector<int> GetMtlIndex() const;
		std::vector<int> GetMeshOffsfets() const;

		void Release();
	private:
		Buffer<Vertex3D> m_vbuffer;

		uint m_meshIndex = 0, m_noVerts = 0;

		std::vector<Mtl> m_mtls;
		std::vector<int> m_mtlIndexes;
		std::vector<int> m_offsets;
		std::string m_name = "unloaded";
		uint m_nofMeshes = 0;
		Shape m_shape;
	};
}

