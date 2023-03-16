#include"pch.h"
#include "RenderShapes.h"

namespace PrimtTech
{
	void RenderBox::Init(ID3D11Device*& device, ID3D11DeviceContext*& dc)
	{
		float distance = 1.f;


		m_shape.reserve(8);
		m_shape.emplace_back(-distance, distance, -distance);
		m_shape.emplace_back(distance, distance, -distance);
		m_shape.emplace_back(-distance, -distance, -distance);
		m_shape.emplace_back(distance, -distance, -distance);
		m_shape.emplace_back(-distance, distance, distance);
		m_shape.emplace_back(distance, distance, distance);
		m_shape.emplace_back(-distance, -distance, distance);
		m_shape.emplace_back(distance, -distance, distance);

		unsigned int indexes[]
		{
			1, 0, 0, 2, 2, 3, 3, 1, // front square
			5, 4, 4, 6, 6, 7, 7, 5, // back square
			0, 4, 1, 5, 2, 6, 3, 7, // connectors
		};

		
		m_vbuffer.CreateVertexBuffer(device, m_shape.data(), m_shape.size(), dc);
		m_ibuffer.CreateIndexBuffer(device, indexes, ARRAYSIZE(indexes));
	}

	void RenderLine::Init(ID3D11Device*& device, ID3D11DeviceContext*& dc)
	{
		m_shape.emplace_back(0.f, 0.f, -.5f);
		m_shape.emplace_back(0.f, 0.f, .5f);

		m_shape[1] = BLUE_3F;

		unsigned int index[]
		{
			0,1
		};
		m_vbuffer.CreateVertexBuffer(device, m_shape.data(), m_shape.size(), dc);
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

	void RenderShape::SetColor(sm::Vector3 color)
	{
		uint size = m_vbuffer.GetBufferSize();
		for (int i = 0; i < size; i++)
		{
			m_vbuffer.Data(i).m_color = color;
		}
		m_vbuffer.MapBuffer();
	}

	void RenderSphere::Init(ID3D11Device*& device, ID3D11DeviceContext*& dc, int points)
	{
		points *= 4;

		const float corner = 0.36f;
		sm::Vector3 color = WHITE_3F;

		const float degreeIncrease = 360.f / (float)points;
		const float radius = .5f;

		//std::vector<BBVertex> vbuffer;

		for (UINT16 i = 0; i < points; i++) // 3 dimensions
		{
			BBVertex vert;
			if (i == (points / 4) * 3)
			{
				//vert.m_color = RED_3F;
				for (int j = 0; j < points; j++)
				{
					float degree = DEG(degreeIncrease * (float)(j + points / 4));
					vert.m_position = { radius * sin(degree), 0.f, radius * cos(degree) };
					m_shape.push_back(vert);
				}
			}
			float degree = DEG(degreeIncrease * (float)(i + points / 4));
			vert.m_position = { radius * cos(degree) , radius * sin(degree), 0.f };
			//vert.m_color = { float(i % 2), float(i % 2), float(i % 2) };
			m_shape.push_back(vert);
		}
		m_shape.push_back(m_shape[0]);

		for (UINT16 i = 0; i < points; i++) // 3 dimensions
		{
			BBVertex vert;

			float degree = DEG(degreeIncrease * (float)(i + points / 4));
			vert.m_position = { 0.f , radius * sin(degree), radius * cos(degree) };
			//vert.m_color = { float(i % 2), float(i % 2), float(i % 2) };
			m_shape.push_back(vert);
		}
		m_shape[0].m_color = { 0.f, 1.f, 0.f };
		m_shape.push_back(m_shape[0]);


		//BBVertex circles[]
		//{
		//	{{0.f, 0.5f, 0.f}, color}, // 0
		//	{{corner, corner, 0.f}, color}, // 1
		//	{{0.5f, 0.f, 0.f}, color}, // 2
		//	{{corner, -corner, 0.f}, color}, // 3
		//	{{0.f, -0.5f, 0.f}, color}, // 4
		//	{{-corner, -corner, 0.f}, color}, // 5
		//	{{-0.5f, 0.f, 0.f}, color}, // 6
		//	{{-corner, corner, 0.f}, color}, // 7

		//	{{0, corner, corner}, color}, // 8
		//	{{0, corner, -corner}, color}, // 9
		//	{{0, 0.f, 0.5f}, color}, // 10 
		//	{{0, 0.f, -0.5f}, color}, // 11
		//	{{0, -corner, corner}, color}, // 12
		//	{{0, -corner, -corner}, color}, // 13

		//	{{-corner, 0.f, -corner}, color}, // 14
		//	{{-corner, 0.f, corner}, color}, // 15
		//	{{corner, 0.f, corner}, color}, // 16
		//	{{corner, 0.f, -corner}, color}, // 17
		//};
		//m_vbuffer.CreateVertexBuffer(device, circles, ARRAYSIZE(circles));
		m_vbuffer.CreateVertexBuffer(device, m_shape.data(), m_shape.size(), NULL, eBufferFlags_IgnoreCreateTwice);
		//unsigned int circleIndexes[]
		//{
		//	0, 1, 2, 3, 4, 5, 6, 7, 0,
		//	8, 10, 12, 4, 13, 11,
		//	14, 6, 15, 10, 16, 2, 17, 11,
		//	9, 0,
		//};
		std::vector<unsigned int> circleIndexes;
		for (UINT16 i = 0; i < m_shape.size(); i++)
		{
			circleIndexes.push_back(i);
		}
		//m_ibuffer.CreateIndexBuffer(device, circleIndexes, ARRAYSIZE(circleIndexes));
		m_ibuffer.CreateIndexBuffer(device, circleIndexes.data(), circleIndexes.size(), NULL, eBufferFlags_IgnoreCreateTwice);
	}
}

