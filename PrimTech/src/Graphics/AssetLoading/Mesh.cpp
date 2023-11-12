#include"pch.h"
#include "Mesh.h"

#include "ResourceHandler.h"
#include "../Utility/CommonDialogs.h"

namespace PrimtTech
{
	Mesh::Mesh(std::string path, ID3D11Device*& device, ID3D11DeviceContext*& deviceContext, bool makeLeftHanded)
	{
		std::vector<Shape> mesh;
		bool check = false;

		switch (1) // In case we don't want to use assimp
		{
		case 0:
			check = FileLoader::LoadObjToBuffer(path, mesh, m_mtls, m_mtlIndexes, true);
			break;
		case 1:
			check = FileLoader::AssimpLoad(path, mesh, m_mtls);
			break;
		case 2: // triangle
		{
			check = true;
			mesh.emplace_back();
			mesh[0].verts.resize(3);

			mesh[0].verts[0].position = float3(-.5f, -.5f, 0.f);
			mesh[0].verts[1].position = float3(0.0f, 0.5f, 0.f);
			mesh[0].verts[2].position = float3(0.5f, -.5f, 0.f);
			for (int i = 0; i < 3; i++)
			{
				mesh[0].verts[i].normal = float3(0.f, 0.f, -1.f);
				mesh[0].verts[i].tangent = float3(0.f, 1.f, 0.f);
				mesh[0].verts[i].bitangent = float3(-1.f, 0.f, 0.f);
			}
			m_mtls.emplace_back();
			m_mtlIndexes.emplace_back(0);
			m_mtlIndexes.emplace_back(3);
		}
		}
		THROW_POPUP_ERROR(check, "loading " + path);
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

		InitInstanceBuffer(10, device, deviceContext);
	}

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

	void Mesh::InitInstanceBuffer(uint numInstances, ID3D11Device*& d, ID3D11DeviceContext*& dc)
	{
		//numInstances = 2;
		if (m_pmeshInstArr)
			delete[] m_pmeshInstArr;
		m_pmeshInstArr = new matrix[numInstances]{matrix(1.f)};

		m_numInstances = numInstances;

		m_instancebuffer.CreateVertexBuffer(d, m_pmeshInstArr, numInstances, dc, eBufferFlags_IgnoreCreateTwice);
		m_instancebuffer.MapBuffer();
	}

	void Mesh::Bind(ID3D11DeviceContext*& dc)
	{
		dc->IASetVertexBuffers(0, 1, m_vbuffer.GetReference(), m_vbuffer.GetStrideP(), &offset[0]);
	}

	void Mesh::InstancedBind(ID3D11DeviceContext*& dc)
	{
		uint strides[] = { m_vbuffer.GetStride(), m_instancebuffer.GetStride() };
		ID3D11Buffer* bufferPtr[] = {m_vbuffer.Get(), m_instancebuffer.Get() };
		dc->IASetVertexBuffers(0, 2, bufferPtr, strides, offset);
	}

	void Mesh::Release()
	{
		m_vbuffer.Release();
		if (m_instancebuffer.Get())
			m_instancebuffer.Release();
		if (m_pmeshInstArr)
			delete[] m_pmeshInstArr;
	}

	void Mesh::ChangeInstance(uint i, matrix instance)
	{
		m_instancebuffer.Data(i) = instance;
	}

	void Mesh::MapInstance()
	{
		m_instancebuffer.MapBuffer();
	}
	int Mesh::IncreaseUses(int n)
	{
		m_numActiveInstances += n;
		if (m_numActiveInstances < 0) m_numActiveInstances = 0;
		return m_numActiveInstances - 1;
	}

	int Mesh::GetNrOfUses() const
	{
		return m_numActiveInstances;
	}

	int Mesh::GetNrOfMaxInstances() const
	{
		return m_numInstances;
	}
}

