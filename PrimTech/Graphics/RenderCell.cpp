#include "RenderCell.h"

RenderCell::RenderCell(int gridResX, int gridResY):
	m_gridRes(d::XMINT2(gridResX, gridResY))
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
	Vertex vertexes[] =
	{
		{ -1.f, -1.f, 1.f, /**/ 0, 0},
		{ -1.f, 0.f, 1, /**/ 0, 1},
		{ 0.f, 0.f, 1.f, /**/ 1, 1},
		{ 0.f, -1.f, 1.f,/**/ 1, 0},
	};

	DWORD indices[] =
	{
		0,1,2,
		0,2,3
	};

	HRESULT hr = m_vBuffer.Init(device, vertexes, ARRAYSIZE(vertexes));
	COM_ERROR(hr, "Failed to setup Vertex Buffer");

	hr = m_iBuffer.Init(device, indices, ARRAYSIZE(indices));
	COM_ERROR(hr, "Failed to setup index Buffer");

	hr = m_cbuffer.Init(device, dc);
	COM_ERROR(hr, "Failed to setup cbuffer");


	UINT offset = 0;
	m_dc->IASetVertexBuffers(0, 1, m_vBuffer.GetReference(), m_vBuffer.GetStrideP(), &offset);
	m_dc->IASetIndexBuffer(m_iBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void RenderCell::SetSwapchain(IDXGISwapChain*& sc)
{
	m_sc = sc;
}

void RenderCell::DrawCell(sm::Vector3 color, sm::Vector2 xy)
{
	m_dc->VSSetConstantBuffers(0, 1, m_cbuffer.GetReference());
	m_cbuffer.getData().color = sm::Vector4(color.x, color.y, color.z, 1.f);

	float coordX = (-m_gridRes.x / 2) + xy.x;
	float coordY = (-m_gridRes.y / 2) + xy.y;

	m_cbuffer.getData().world = d::XMMatrixTranspose(d::XMMatrixTranslationFromVector({ coordX, coordY }));

	m_cbuffer.applyChange();
	m_dc->DrawIndexed(m_iBuffer.GetBufferSize(), 0, 0);
}

void RenderCell::SetViewProjM(sm::Matrix m)
{
	m_cbuffer.getData().viewProj = d::XMMatrixTranspose(m);
}
