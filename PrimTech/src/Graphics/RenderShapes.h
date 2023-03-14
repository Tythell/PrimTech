#pragma once
#include "../old/Model.h"

namespace PrimtTech
{
	class RenderShape
	{
	public:
		virtual void Draw(ID3D11DeviceContext*& dc);
		//virtual void Init(ID3D11Device*& device, ID3D11DeviceContext*& dc) = 0;
	protected:
		Buffer<BBVertex> m_vbuffer;
		Buffer<unsigned int> m_ibuffer;
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

		void SetLine(sm::Vector3 start, sm::Vector3 end);
	};

	class RenderBox : public RenderShape
	{
	public:
		// Inherited via RenderShape
		virtual void Init(ID3D11Device*& device, ID3D11DeviceContext*& dc);
	};
}

