#pragma once
#include<d3d11.h>
#include"../Utility/Popup.h"
#include"CbufferTypes.h"


enum BufferType : int
{
	eVERTEX = D3D11_BIND_VERTEX_BUFFER, 
	eCONSTANT = D3D11_BIND_INDEX_BUFFER, 
	eINDEX = D3D11_BIND_CONSTANT_BUFFER
};
enum BufferUsage
{
	eDEFAULT = D3D11_USAGE_DEFAULT, eIMMULATBLE = D3D11_USAGE_IMMUTABLE, eDYNAMIC = D3D11_USAGE_DYNAMIC
};

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
	T& Data(UINT index = 0)
	{
		return m_data[index];
	}
	ID3D11Buffer* Get()const
	{
		return m_buffer;
	}
	ID3D11Buffer* const* GetReference()const
	{
		return &m_buffer;
	}
	// DeviceContext only needed if buffer is dynamic
	HRESULT CreateVertexBuffer(ID3D11Device*& device, T* data, UINT bufferSize, ID3D11DeviceContext* dc = NULL)
	{
		POPUP_ERROR((m_buffer == nullptr), "Buffer created twice");
		m_type = eVERTEX;

		m_usage = eIMMULATBLE;
		UINT cpuFlags = 0;
		m_data = new T[bufferSize];
		for (int i = 0; i < bufferSize; i++)
		{
			m_data[i] = data[i];
		}
		if (dc)
		{
			m_usage = eDYNAMIC;
			mp_dc = dc;
			cpuFlags = D3D11_CPU_ACCESS_WRITE;
		}

		m_bufferSize = bufferSize;

		//m_data = data;
		mp_dc = dc;
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.Usage = (D3D11_USAGE)m_usage;
		bufferDesc.ByteWidth = m_stride * m_bufferSize;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = cpuFlags;
		bufferDesc.MiscFlags = 0;
		bufferDesc.Usage = (D3D11_USAGE)m_usage;

		D3D11_SUBRESOURCE_DATA bufferData;
		ZeroMemory(&bufferData, sizeof(D3D11_SUBRESOURCE_DATA));
		bufferData.pSysMem = data;

		return HRESULT(device->CreateBuffer(&bufferDesc, &bufferData, &m_buffer));
	}

	HRESULT CreateConstantBuffer(ID3D11Device*& device, ID3D11DeviceContext*& dc)
	{
		m_type = eCONSTANT;
		m_usage = eDYNAMIC;
		if (m_buffer)
		{
			Popup::Error("Buffer created twice");
			throw;
		}
		//m_type = BufferType::eCONSTANT;

		mp_dc = dc;
		m_data = new T;
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
	// DeviceContext only needed if buffer will be changed
	HRESULT CreateIndexBuffer(ID3D11Device*& device, T* indexData, UINT numIndices, ID3D11DeviceContext* dc = NULL)
	{
		m_type = eINDEX;
		m_usage = eIMMULATBLE;
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
		mp_dc = dc;
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

	HRESULT CreateBuffer(BufferType type, ID3D11Device*& device, T* data, UINT size, ID3D11DeviceContext* dc = NULL)
	{
		m_type = type;
		m_bufferSize = size;
		mp_dc = dc;
		m_usage = eIMMULATBLE;
		if (m_buffer)
		{
			throw;
			Popup::Error("Buffer created twice");
		}
		UINT cpuFlags = 0;
		m_data = new T[size];
		for (int i = 0; i < size; i++)
		{
			m_data[i] = data[i];
		}
		if (dc)
		{
			m_usage = eDYNAMIC;
			mp_dc = dc;
			cpuFlags = D3D11_CPU_ACCESS_WRITE;
		}

		//m_data = data;
		mp_dc = dc;
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.Usage = (D3D11_USAGE)m_usage;
		bufferDesc.ByteWidth = m_stride * m_bufferSize;
		bufferDesc.BindFlags = (UINT)type;
		bufferDesc.CPUAccessFlags = cpuFlags;
		bufferDesc.MiscFlags = 0;
		bufferDesc.Usage = (UINT)m_usage;

		D3D11_SUBRESOURCE_DATA bufferData;
		ZeroMemory(&bufferData, sizeof(D3D11_SUBRESOURCE_DATA));
		bufferData.pSysMem = data;

		return HRESULT(device->CreateBuffer(&bufferDesc, &bufferData, &m_buffer));
	}

	void UpdateBuffer()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = mp_dc->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		COM_ERROR(hr, "Failed to update CB");
		CopyMemory(mappedResource.pData, m_data, sizeof(T) * m_bufferSize);
		mp_dc->Unmap(m_buffer, 0);
	}
	ID3D11DeviceContext* GetDC() const
	{
		return mp_dc; // Only works if you initilized buffer with device context
	}
private:
	T* m_data = nullptr;
	ID3D11Buffer* m_buffer = nullptr;
	ID3D11DeviceContext* mp_dc = nullptr;
	UINT m_stride = sizeof(T);
	UINT m_bufferSize = 0;
	BufferUsage m_usage = eDEFAULT;
	BufferType m_type = BufferType(0);
};
