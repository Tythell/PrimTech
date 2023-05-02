#include"pch.h"
#include "Mesh.h"

#include "ResourceHandler.h"
#include "../Utility/CommonDialogs.h"
#undef min
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace PrimtTech
{
	//sm::Vector3 aiVecToSm(const aiVector3D aivec)
	//{
	//	return sm::Vector3(aivec.x, aivec.y, aivec.z);
	//}
	//sm::Vector2 aiVecToSm(const aiVector2D aivec)
	//{
	//	return sm::Vector2(aivec.x, aivec.y);
	//}

	Mesh::Mesh(std::string path, ID3D11Device*& device, bool makeLeftHanded)
	{
		ID3D11Buffer* buffer = nullptr;
		ID3D11Buffer** gee = m_vbuffer.GetInArrayWith(buffer);

		std::vector<Shape> mesh;
		bool check = false;
		uchar* dummy = NULL;
		

		switch (1) // In case we don't want to use assimp
		{
		case 0:
			check = FileLoader::LoadObjToBuffer(path, mesh, m_mtls, m_mtlIndexes, makeLeftHanded);
			break;
		case 1:
			check = FileLoader::AssimpLoad(path, mesh, m_mtls);
			break;
		case 2: // triangle
		{
			check = true;
			mesh.emplace_back();
			mesh[0].verts.resize(3);

			mesh[0].verts[0].position = sm::Vector3(-.5f, -.5f, 0.f);
			mesh[0].verts[1].position = sm::Vector3(0.0f, 0.5f, 0.f);
			mesh[0].verts[2].position = sm::Vector3(0.5f, -.5f, 0.f);
			for (int i = 0; i < 3; i++)
			{
				mesh[0].verts[i].normal = sm::Vector3(0.f, 0.f, -1.f);
				mesh[0].verts[i].tangent = sm::Vector3(0.f, 1.f, 0.f);
				mesh[0].verts[i].bitangent = sm::Vector3(-1.f, 0.f, 0.f);
			}
			m_mtls.emplace_back();
			m_mtlIndexes.emplace_back(0);
			m_mtlIndexes.emplace_back(3);
		}
		}
		THROW_POPUP_ERROR(check, " loading" + path);
		m_name = StringHelper::GetName(path);
		m_nofMeshes = mesh.size();
		m_offsets.emplace_back(0);
		int lastSize = 0;
		for (int i = 0; i < m_nofMeshes; i++)
		{
			lastSize += mesh[i].verts.size();
			m_offsets.emplace_back(lastSize);
		}

		uint totalVertCount = 0;
		for (int i = 0; i < m_nofMeshes; i++)
			totalVertCount += mesh[i].verts.size();

		m_shape.verts.reserve(totalVertCount);

		for (int i = 0; i < m_nofMeshes; i++)
			m_shape.verts.insert(m_shape.verts.end(), mesh[i].verts.begin(), mesh[i].verts.end());

		UINT bsize = m_shape.verts.size();

		HRESULT hr = m_vbuffer.CreateVertexBuffer(device, m_shape.verts.data(), bsize);
		COM_ERROR(hr, "Failed to load vertex buffer");
	}

	//Mesh::Mesh(const Mesh& other)
	//{
	//	ID3D11Device* pDev = other.pDevice;
	//	Vertex3D verts = other.m_shape.verts
	//
	//	m_vbuffer.CreateVertexBuffer(pDev, other.m_shape.verts.data(), other.m_vbuffer.GetBufferSize());
	//}

	Buffer<Vertex3D>& Mesh::GetVBuffer()
	{
		return m_vbuffer;
	}

	std::string Mesh::GetName() const
	{
		return m_name;
	}

	const UINT Mesh::GetNofMeshes() const
	{
		return m_nofMeshes;
	}

	std::vector<Mtl> Mesh::GetMtl() const
	{
		return m_mtls;
	}

	std::vector<int> Mesh::GetMtlIndex() const
	{
		return m_mtlIndexes;
	}

	std::vector<int> Mesh::GetMeshOffsfets() const
	{
		return m_offsets;
	}

	void Mesh::Release()
	{
		m_vbuffer.Release();
	}

}

