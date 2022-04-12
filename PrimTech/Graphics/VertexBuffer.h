#pragma once
#include <d3d11.h>
#include <memory>

template<class T>
class VertexBuffer
{
private:
	VertexBuffer(const VertexBuffer<T>& rhs);

	ID3D11Buffer* buffer;
	std::unique_ptr<UINT> stride;
	UINT bufferSize = 0;
public:
	VertexBuffer() {}
	~VertexBuffer()
	{
		buffer->Release();
	}

	ID3D11Buffer* Get()const
	{
		return buffer;
	}
	ID3D11Buffer* const* GetReference()const
	{
		return &buffer;
	}
	UINT GetBufferSize() const
	{
		return this->bufferSize;
	}
	const UINT GetStride() const
	{
		return *this->stride.get();
	}
	const UINT* GetStrideP() const
	{
		return this->stride.get();
	}
	HRESULT Init(ID3D11Device*& device, T* data, UINT numVertices)
	{
		this->bufferSize = numVertices;
		this->stride = std::make_unique<UINT>(sizeof(T));
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(T) * numVertices;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;


		return HRESULT(device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &this->buffer));
	}
};