#pragma once
#include <d3d11.h>
class IndexBuffer
{
private:
	ID3D11Buffer* m_buffer;
	UINT bufferSize = 0;
public:
	IndexBuffer() {}

	ID3D11Buffer* Get()const
	{
		return m_buffer;
	}
	ID3D11Buffer* const* GetRef()const
	{
		return &m_buffer;
	}
	UINT GetBufferSize() const
	{
		return this->bufferSize;
	}
	HRESULT Init(ID3D11Device* device, DWORD* data, UINT numIndices)
	{
		this->bufferSize = numIndices;
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * numIndices;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferData;
		indexBufferData.pSysMem = data;
		HRESULT hr = device->CreateBuffer(&indexBufferDesc, &indexBufferData, &this->m_buffer);
		return hr;
	}
};