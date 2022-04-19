#pragma once
#include "Math\Math.h"
#include "Graphics/Vertex.h"
#include"Graphics\IndexBuffer.h"
#include"Graphics\VertexBuffer.h"
#include"Graphics\ConstantBuffer.h"

class RenderGrid
{
public:
	RenderGrid();

	VertexBuffer<Vertex> m_vBuffer;
	IndexBuffer m_iBuffer;

	void Init(d::XMINT2 windowRes, d::XMINT2 gridRes, ID3D11Device*& device, ID3D11DeviceContext*& dc);
	void DrawGrid();

	void DrawCell(sm::Vector4 color, sm::Vector2 xy);
	void SetViewProjM(sm::Matrix m);
private:
	int m_cellSize;
	
	ID3D11DeviceContext* m_dc;
	ConstantBuffer<Transforms> m_cbuffer;
};