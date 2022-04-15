#pragma once
#include <d3d11.h>
#include"../Utility/Popup.h"

template<class T>
class ConstantBuffer
{
private:
	ConstantBuffer(const ConstantBuffer<T>& rhs);

private:
	ID3D11Buffer* m_buffer;
	ID3D11DeviceContext* m_dc = nullptr;
	T data;
	bool m_on = false;
public:
	ConstantBuffer() {}
	~ConstantBuffer()
	{
		if(m_on)
			m_buffer->Release();
	}

	T& getData()
	{
		return data;
	}

	ID3D11Buffer* Get()const
	{
		return m_buffer;
	}
	ID3D11Buffer* const* GetReference()const
	{
		return &m_buffer;
	}
	HRESULT Init(ID3D11Device*& device, ID3D11DeviceContext*& deviceContext)
	{
		m_on = true;
		m_dc = deviceContext;
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
		desc.StructureByteStride = 0;
		HRESULT hr = device->CreateBuffer(&desc, 0, &m_buffer);
		return hr;
	}

	bool applyChange()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = m_dc->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		COM_ERROR(hr, "Mapping cbuffer failed");
		CopyMemory(mappedResource.pData, &data, sizeof(T));
		this->m_dc->Unmap(m_buffer, 0);
		return true;
	}
};