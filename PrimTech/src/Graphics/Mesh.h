#pragma once
#include "Transform.h"
#include"Buffer.h"
#include"Vertex.h"
#include "Material.h"

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

// Contains Vertex data
class Mesh
{
public:
	Mesh(std::string path, ID3D11Device*& device, bool makeLeftHanded = true);

	Buffer<Vertex3D>& GetVBuffer();
	std::string GetName() const;
	void IncreaseUses();
	void DecreaseUses();
	void ResetUses();
	int GetNrOfUses() const;
	d::BoundingSphere GetBSphere() const;
	const UINT GetNofMeshes() const;
	std::vector<Mtl> GetMtl() const;
	std::vector<int> GetMtlIndex() const;
	std::vector<int> GetMeshOffsfets() const;
private:
	Buffer<Vertex3D> m_vbuffer;
	std::vector<Mtl> m_mtls;
	std::vector<int> m_mtlIndexes;
	std::vector<int> m_offsets;
	std::string m_name = "unloaded";
	int m_nrOfUses = 0;
	d::BoundingSphere m_bsphere;
	UINT m_nofMeshes = 0;
	Shape m_shape;
};