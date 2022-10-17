#include "MayaMesh.h"

pt::MayaMesh::MayaMesh(std::vector<Vertex3D> vArray, ID3D11Device*& device, ID3D11DeviceContext*& dc):
	device(device), dc(dc)
{
	m_vBuffer.CreateVertexBuffer(device, vArray.data(), vArray.size(), dc);
}

void pt::MayaMesh::RefreshMesh(std::vector<Vertex3D> vArray)
{
	m_vBuffer.RefreshVertexBuffer(device, vArray.data(), vArray.size(), dc);
}

void pt::MayaMesh::Draw()
{
	m_mat.Set(dc);
	dc->Draw(m_vBuffer.GetBufferSize(), 0);
}
