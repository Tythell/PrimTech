#include "RenderCell.h"

RenderCell::RenderCell(int gridResX, int gridResY):
	m_gridRes(d::XMINT2(gridResX, gridResY)),


	m_windRes(d::XMINT2(0,0))
{
	if (gridResX == 0 || gridResY == 0)
	{
		Popup::Error("Cell grid not high enough, program will exit");
		throw;
	}
}

void RenderCell::Init(ID3D11Device*& device, ID3D11DeviceContext*& dc)
{
	m_dc = dc;
	VertexQuad vertexes[] =
	{
		{ -1.f, -1.f},
		{ -1.f, 0.f},
		{ 0.f, 0.f},
		{ 0.f, -1.f},
	};

	DWORD indices[] =
	{
		0,1,2,
		0,2,3
	};

	HRESULT hr = m_vBuffer.CreateVertexBuffer(device, vertexes, ARRAYSIZE(vertexes));
	COM_ERROR(hr, "Failed to setup Vertex Buffer");

	hr = m_iBuffer.CreateIndexBuffer(device, indices, ARRAYSIZE(indices));
	COM_ERROR(hr, "Failed to setup index Buffer");

	hr = m_cBuffer.CreateConstantBuffer(device, dc);
	COM_ERROR(hr, "Failed to setup cbuffer");


	UINT offset = 0;
	m_dc->IASetVertexBuffers(0, 1, m_vBuffer.GetReference(), m_vBuffer.GetStrideP(), &offset);
	m_dc->IASetIndexBuffer(m_iBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	m_dc->VSSetConstantBuffers(0, 1, m_cBuffer.GetReference());
}

void RenderCell::DrawCell(sm::Vector3 color, sm::Vector2 xy)
{
	m_cBuffer.Data().color = sm::Vector4(color.x, color.y, color.z, 1.f);

	float coordX = (-m_gridRes.x / 2) + xy.x;
	float coordY = (-m_gridRes.y / 2) + xy.y;

	m_cBuffer.Data().world = d::XMMatrixTranspose(d::XMMatrixTranslationFromVector({ coordX, coordY }));

	m_cBuffer.UpdateCB();
	m_dc->DrawIndexed(m_iBuffer.GetBufferSize(), 0, 0);
}

void RenderCell::SetViewProjM(sm::Matrix m)
{
	m_cBuffer.Data().viewProj = d::XMMatrixTranspose(m);
}
