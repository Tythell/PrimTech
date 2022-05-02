#pragma once
#include "../Math\Math.h"
#include "Vertex.h"
#include"IndexBuffer.h"
#include"VertexBuffer.h"
#include"ConstantBuffer.h"

class RenderCell
{
public:
	RenderCell(int gridResX, int gridResY);

	void Init(ID3D11Device*& device, ID3D11DeviceContext*& dc);
	void SetSwapchain(IDXGISwapChain*& sc);

	void DrawCell(sm::Vector3 color, sm::Vector2 xy);
	void SetViewProjM(sm::Matrix m);
private:
	int m_cellSize = 0;
	VertexBuffer<Vertex> m_vBuffer;
	IndexBuffer m_iBuffer;
	ID3D11DeviceContext* m_dc = nullptr;
	ConstantBuffer<Transforms> m_cbuffer;
	IDXGISwapChain* m_sc = nullptr;

	d::XMINT2 m_gridRes;
	d::XMINT2 m_windRes;
};