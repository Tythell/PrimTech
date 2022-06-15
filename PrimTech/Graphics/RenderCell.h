#pragma once
#include "../Math\Math.h"
#include "Vertex.h"
#include"IndexBuffer.h"
#include"VertexBuffer.h"
#include"ConstantBuffer.h"
#include"Buffer.h"

class RenderCell
{
public:
	RenderCell(int gridResX, int gridResY);

	void Init(ID3D11Device*& device, ID3D11DeviceContext*& dc);

	void DrawCell(sm::Vector3 color, sm::Vector2 xy);
	void SetViewProjM(sm::Matrix m);
private:
	int m_cellSize = 0;
	Buffer<VertexQuad> m_vBuffer;
	Buffer<DWORD> m_iBuffer;
	Buffer<hlsl::cbTransformsQuad> m_cBuffer;


	ID3D11DeviceContext* m_dc = nullptr;

	d::XMINT2 m_gridRes;
	d::XMINT2 m_windRes;
};