#pragma once
#include "AssetLoading/Mesh.h"

namespace PrimtTech
{
	class RenderShape
	{
	public:
		~RenderShape();
		virtual void Draw(ID3D11DeviceContext*& dc);
		void DrawShape(ID3D11DeviceContext*& dc) { dc->DrawIndexed(m_ibuffer.GetBufferSize(), 0, 0); }
		void SetColor(float3 color);
		void SetBuffer(ID3D11DeviceContext*& dc)
		{
			uint offset = 0;
			dc->IASetVertexBuffers(0, 1, m_vbuffer.GetReference(), m_vbuffer.GetStrideP(), &offset);
			dc->IASetIndexBuffer(m_ibuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		}
	protected:
		Buffer<BBVertex> m_vbuffer;
		Buffer<unsigned int> m_ibuffer;
		std::vector<BBVertex> m_shape;
	};

	class RenderSphere : public RenderShape
	{
	public:
		// Inherited via RenderShape
		virtual void Init(ID3D11Device*& device, ID3D11DeviceContext*& dc, int points);
	};

	class RenderLine : public RenderShape
	{
	public:
		// Inherited via RenderShape
		virtual void Init(ID3D11Device*& device, ID3D11DeviceContext*& dc);

		void SetLine(float3 start, float3 end);
	};

	class RenderBox : public RenderShape
	{
	public:
		// Inherited via RenderShape
		virtual void Init(ID3D11Device*& device, ID3D11DeviceContext*& dc);
	};
}

