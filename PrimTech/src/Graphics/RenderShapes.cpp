#include "RenderShapes.h"

void RenderBox::Init(ID3D11Device*& device, ID3D11DeviceContext*& dc)
{
	BBVertex vertex[]
	{
		{{-0.5f, 0.5f, -0.5f }, WHITE_3F}, //F TL	 0
		{{0.5f, 0.5f, -0.5f }, WHITE_3F}, //F TR	 1
		{{-0.5f, -0.5f, -0.5f }, WHITE_3F}, //F BL 2
		{{0.5f, -0.5f, -0.5f }, WHITE_3F}, //F BR	 3
		{{-0.5f, 0.5f, 0.5f }, WHITE_3F}, //B TL	 4
		{{0.5f, 0.5f, 0.5f }, WHITE_3F}, //B TR	 5
		{{-0.5f, -0.5f, 0.5f }, WHITE_3F}, //B BL	 6
		{{0.5f, -0.5f, 0.5f }, WHITE_3F}, //B BR	 7
	};

	unsigned int indexes[]
	{
		1, 0, 0, 2, 2, 3, 3, 1, // front square
		5, 4, 4, 6, 6, 7, 7, 5, // back square
		0, 4, 1, 5, 2, 6, 3, 7, // connectors
	};

	m_vbuffer.CreateVertexBuffer(device, vertex, ARRAYSIZE(vertex));
	m_ibuffer.CreateIndexBuffer(device, indexes, ARRAYSIZE(indexes));
}

void RenderLine::Init(ID3D11Device*& device, ID3D11DeviceContext*& dc)
{
	BBVertex line[]
	{
		{{0.f, 0.f, -.5f}, WHITE_3F},
		{{0.f, 0.f, .5f}, BLUE_3F},
	};
	unsigned int index[]
	{
		0,1
	};
	m_vbuffer.CreateVertexBuffer(device, line, ARRAYSIZE(line), dc);
	m_ibuffer.CreateIndexBuffer(device, index, ARRAYSIZE(index));
}

void RenderLine::SetLine(sm::Vector3 start, sm::Vector3 end)
{
	m_vbuffer.Data(0).m_position = start;
	m_vbuffer.Data(0).m_color = WHITE_3F;
	m_vbuffer.Data(1).m_position = end;
	m_vbuffer.Data(1).m_color = BLUE_3F;
	m_vbuffer.MapBuffer();
}

void RenderShape::Draw(ID3D11DeviceContext*& dc)
{
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, m_vbuffer.GetReference(), m_vbuffer.GetStrideP(), &offset);
	dc->IASetIndexBuffer(m_ibuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	dc->DrawIndexed(m_ibuffer.GetBufferSize(), 0, 0);
}

void RenderSphere::Init(ID3D11Device*& device, ID3D11DeviceContext*& dc)
{
	const float corner = 0.36f;
	sm::Vector3 color = WHITE_3F;
	BBVertex circles[]
	{
		{{0.f, 0.5f, 0.f}, color}, // 0
		{{corner, corner, 0.f}, color}, // 1
		{{0.5f, 0.f, 0.f}, color}, // 2
		{{corner, -corner, 0.f}, color}, // 3
		{{0.f, -0.5f, 0.f}, color}, // 4
		{{-corner, -corner, 0.f}, color}, // 5
		{{-0.5f, 0.f, 0.f}, color}, // 6
		{{-corner, corner, 0.f}, color}, // 7

		{{0, corner, corner}, color}, // 8
		{{0, corner, -corner}, color}, // 9
		{{0, 0.f, 0.5f}, color}, // 10 
		{{0, 0.f, -0.5f}, color}, // 11
		{{0, -corner, corner}, color}, // 12
		{{0, -corner, -corner}, color}, // 13

		{{-corner, 0.f, -corner}, color}, // 14
		{{-corner, 0.f, corner}, color}, // 15
		{{corner, 0.f, corner}, color}, // 16
		{{corner, 0.f, -corner}, color}, // 17
	};
	m_vbuffer.CreateVertexBuffer(device, circles, ARRAYSIZE(circles));
	unsigned int circleIndexes[]
	{
		0, 1, 2, 3, 4, 5, 6, 7, 0,
		8, 10, 12, 4, 13, 11,
		14, 6, 15, 10, 16, 2, 17, 11,
		 9, 0,
	};
	m_ibuffer.CreateIndexBuffer(device, circleIndexes, ARRAYSIZE(circleIndexes));
}