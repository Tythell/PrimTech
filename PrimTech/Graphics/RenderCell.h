#pragma once
#include "../Math\Math.h"
#include "Vertex.h"
#include"IndexBuffer.h"
#include"VertexBuffer.h"
#include"ConstantBuffer.h"

class RenderCell
{
public:
	RenderCell();

	void Init(d::XMINT2 windowRes, d::XMINT2 gridRes, ID3D11Device*& device, ID3D11DeviceContext*& dc);

	void DrawCell(sm::Vector4 color, sm::Vector2 xy);
	void SetViewProjM(sm::Matrix m);
private:
	int m_cellSize;
	VertexBuffer<Vertex> m_vBuffer;
	IndexBuffer m_iBuffer;
	ID3D11DeviceContext* m_dc;
	ConstantBuffer<Transforms> m_cbuffer;
};