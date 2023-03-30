#pragma once
#include "Math/Math.h"
#include"Buffer.h"
#include"Vertex.h"
#include "Material.h"

namespace PrimtTech
{
	struct Mtl
	{
		std::string name;
		std::string diffuseName; // map_Kd
	};

	struct Shape
	{
		std::vector<Vertex3D> verts;
		//UINT mtlIndex = 0;
		//Mtl material;
	};

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
	private:
		Buffer<Vertex3D> m_vbuffer;
		std::vector<Mtl> m_mtls;
		std::vector<int> m_mtlIndexes;
		std::vector<int> m_offsets;
		std::string m_name = "unloaded";
		uint m_nofMeshes = 0;
		Shape m_shape;
	};
}

