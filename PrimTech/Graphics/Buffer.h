#pragma once
#include<d3d11.h>
#include"../Utility/Popup.h"


//enum BufferType
//{
//	eVERTEX = D3D11_BIND_VERTEX_BUFFER, 
//	eCONSTANT = D3D11_BIND_INDEX_BUFFER, 
//	eINDEX = D3D11_BIND_CONSTANT_BUFFER
//};
//enum BufferUsage
//{
//	eDEFAULT, eIMMULATBLE, eDYNAMIC
//};

template<class T>
class Buffer
{
public:
	Buffer() {}
	~Buffer()
	{
		m_buffer->Release();
		if(m_data != nullptr) 
			delete m_data;
	}

	UINT GetStride() const
	{
		return m_stride;
	}
	UINT* GetStrideP()
	{
		return &m_stride;
	}
	UINT GetBufferSize() const
	{
		return m_bufferSize;
	}
	T& Data()
	{
		return *m_data;
	}
	ID3D11Buffer* Get()const
	{
		return m_buffer;
	}
	ID3D11Buffer* const* GetReference()const
	{
		return &m_buffer;
	}
	
	HRESULT CreateVertexBuffer(ID3D11Device*& device, T* data, UINT bufferSize)
	{
		if (m_buffer)
		{
			throw;
			Popup::Error("Buffer created twice");
		}
		m_bufferSize = bufferSize;
		m_stride = sizeof(T);

		//m_data = data;
		
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.ByteWidth = sizeof(T) * m_bufferSize;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA bufferData;
		ZeroMemory(&bufferData, sizeof(D3D11_SUBRESOURCE_DATA));
		bufferData.pSysMem = data;

		return HRESULT(device->CreateBuffer(&bufferDesc, &bufferData, &m_buffer));
	}

	HRESULT CreateConstantBuffer(ID3D11Device*& device, ID3D11DeviceContext*& dc)
	{
		if (m_buffer)
		{
			Popup::Error("Buffer created twice");
			throw;
		}
		//m_type = BufferType::eCONSTANT;

		mp_dc = dc;
		m_data = new T;
		m_stride = sizeof(T);
		m_bufferSize = 1;

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - sizeof(T) % 16));
		bufferDesc.StructureByteStride = 0;
		
		return HRESULT(device->CreateBuffer(&bufferDesc, NULL, &m_buffer));
	}

	HRESULT CreateIndexBuffer(ID3D11Device*& device, T* indexData, UINT numIndices)
	{
		if (m_buffer)
		{
			Popup::Error("Buffer created twice");
			throw;
		}
		else if (sizeof(T) != sizeof(DWORD))
		{
			Popup::Error("Indexbuffer type must be DWORD");
			throw;
		}
			
		//m_type = BufferType::eINDEX

		m_bufferSize = numIndices;
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.ByteWidth = sizeof(T) * numIndices;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA bufferData;
		ZeroMemory(&bufferData, sizeof(D3D11_SUBRESOURCE_DATA));
		bufferData.pSysMem = indexData;

		return HRESULT(device->CreateBuffer(&bufferDesc, &bufferData, &m_buffer));
	}

	void UpdateCB()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = mp_dc->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		COM_ERROR(hr, "Failed to update CB");
		CopyMemory(mappedResource.pData, m_data, sizeof(T));
		mp_dc->Unmap(m_buffer, 0);
	}
private:
	T* m_data = nullptr;
	ID3D11Buffer* m_buffer = nullptr;
	ID3D11DeviceContext* mp_dc = nullptr;
	UINT m_stride = 0;
	UINT m_bufferSize = 0;
	//BufferUsage m_usage;
	//BufferType m_type;
};
