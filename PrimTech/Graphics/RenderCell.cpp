#include "RenderCell.h"

RenderCell::RenderCell()
{
}

void RenderCell::Init(d::XMINT2 windowRes, d::XMINT2 gridRes, ID3D11Device*& device, ID3D11DeviceContext*& dc)
{
	m_dc = dc;
	Vertex vertexes[] =
	{
		{ -0.5f, -0.5f, 1.f, /**/ 0, 0},
		{ -0.5, 0.5f, 1, /**/ 0, 1},
		{ 0.5, 0.5f, 1.f, /**/ 1, 1},
		{ 0.5f, -0.5f, 1.f,/**/ 1, 0},
	};

	DWORD indices[] =
	{
		0,1,2,
		0,2,3
	};

	HRESULT hr = m_vBuffer.Init(device, vertexes, ARRAYSIZE(vertexes));

	m_iBuffer.Init(device, indices, ARRAYSIZE(indices));


	COM_ERROR(hr, "Failed to setup Vertex Buffer");


	UINT offset = 0;
	m_dc->IASetVertexBuffers(0, 1, m_vBuffer.GetReference(), m_vBuffer.GetStrideP(), &offset);
	m_dc->IASetIndexBuffer(m_iBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void RenderCell::DrawCell(sm::Vector4 color, sm::Vector2 xy)
{
	m_cbuffer.getData().world = d::XMMatrixTranspose(d::XMMatrixTranslationFromVector(xy));
	m_cbuffer.getData().color = color;
	m_cbuffer.applyChange();
	m_dc->DrawIndexed(m_iBuffer.GetBufferSize(), 0, 0);
}

void RenderCell::SetViewProjM(sm::Matrix m)
{
	m_cbuffer.getData().viewProj = d::XMMatrixTranspose(m);
}
