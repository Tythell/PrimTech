#pragma once
#include<d3d11.h>
#include"../Utility/Popup.h"
#include"CbufferTypes.h"

namespace PrimtTech
{
	enum BufferType : int
	{
		eVERTEX = D3D11_BIND_VERTEX_BUFFER,
		eCONSTANT = D3D11_BIND_CONSTANT_BUFFER,
		eINDEX = D3D11_BIND_INDEX_BUFFER
	};
	enum BufferUsage
	{
		eDEFAULT = D3D11_USAGE_DEFAULT,
		eIMMULATBLE = D3D11_USAGE_IMMUTABLE,
		eDYNAMIC = D3D11_USAGE_DYNAMIC
	};

	enum BufferFlags
	{
		eBufferFlags_IgnoreCreateTwice = 0x1,
		eBufferFlags_Borrowoingdata,
	};

	template<class T>
	class Buffer
	{
	public:
		Buffer() {}
		/*//Buffer(const Buffer& other) = delete;
		//{
			//m_type = other.m_type;
			//m_bufferSize = other.m_bufferSize;
			//m_data = other.m_data;
			//m_stride = other.m_stride;
			//m_usage = other.m_usage;


			//switch (m_type)
			//{
			//case eVERTEX:
			//{
			//	mp_device = other.mp_device;
			//	mp_dc = other.mp_dc;

			//	CreateVertexBuffer(mp_device, other.m_data, other.m_bufferSize, mp_dc);
			//	break;
			//	
			//}
			//case eCONSTANT:
			//{
			//	mp_device = other.mp_device;
			//	mp_dc = other.mp_dc;

			//	CreateConstantBuffer(mp_device, mp_dc);
			//	break;
			//}
			//case eINDEX:
			//{
			//	mp_device = other.mp_device;
			//	mp_dc = other.mp_dc;

			//	CreateVertexBuffer(mp_device, other.m_data, other.m_bufferSize, mp_dc);
			//	break;
			//}
			//default:
			//	break;
			//}
		//}*/
		~Buffer()
		{
			m_buffer->Release();
			if (m_srv)
				m_srv->Release();
			if (!(m_flags & eBufferFlags_Borrowoingdata) && (m_data && (m_type != eVERTEX)))
				delete m_data;
		}

		void Release()
		{
			//m_buffer->Release();
			//if (m_data && m_type != eVERTEX)
			//	delete m_data;
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
		HRESULT CreateVertexBuffer(ID3D11Device*& device, T* data, UINT bufferSize, ID3D11DeviceContext* dc = NULL, unsigned char flags = 0)
		{
			mp_device = device;
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

			if (flags & eBufferFlags_IgnoreCreateTwice && m_buffer)
			{
				m_buffer->Release();
			}
			else
				THROW_POPUP_ERROR((m_buffer == nullptr), "Buffer created twice");
			m_type = eVERTEX;

			m_usage = eIMMULATBLE;
			bufferDesc.CPUAccessFlags = 0;
			//m_data = new T[bufferSize];
			//for (int i = 0; i < bufferSize; i++)
			//{
			//	m_data[i] = data[i];
			//}
			m_data = data;
			if (dc)
			{
				m_usage = eDYNAMIC;
				mp_dc = dc;
				bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			}

			m_bufferSize = bufferSize;

			//m_data = data;
			mp_dc = dc;

			bufferDesc.Usage = (D3D11_USAGE)m_usage;
			bufferDesc.ByteWidth = m_stride * m_bufferSize;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.MiscFlags = 0;
			bufferDesc.Usage = (D3D11_USAGE)m_usage;

			D3D11_SUBRESOURCE_DATA bufferData;
			ZeroMemory(&bufferData, sizeof(D3D11_SUBRESOURCE_DATA));
			bufferData.pSysMem = data;

			HRESULT hr = device->CreateBuffer(&bufferDesc, &bufferData, &m_buffer);

			return hr;
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
		HRESULT CreateStructuredBuffer(ID3D11Device*& device, T* data, uint bufferSize, ID3D11DeviceContext*& dc)
		{
			m_flags |= eBufferFlags_Borrowoingdata;
			m_usage = eDYNAMIC;
			if (m_buffer)
			{
				Popup::Error("Buffer created twice");
				throw;
			}
			//m_type = BufferType::eCONSTANT;

			mp_dc = dc;
			m_data = data;
			m_bufferSize = bufferSize;
			m_stride = sizeof(T);

			UINT mod = sizeof(T) % 16u;
			UINT byteSize = (mod == 0) ? static_cast<UINT>(sizeof(T)) : static_cast<UINT>(sizeof(T) + (16u - mod));

			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth = byteSize * m_bufferSize;
			bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			bufferDesc.StructureByteStride = byteSize;


			HRESULT hr = device->CreateBuffer(&bufferDesc, NULL, &m_buffer);
			COM_ERROR(hr, "failed to setup sbuffer");

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.FirstElement = 0;
			srvDesc.Buffer.NumElements = m_bufferSize;

			hr = device->CreateShaderResourceView(m_buffer, &srvDesc, &m_srv);

			COM_ERROR(hr, "failed to setup srv");

			return SUCCEEDED(hr);
		}

		void BindSRV(uint slot)
		{
			mp_dc->PSSetShaderResources(slot, 1, &m_srv);
		}

		// DeviceContext only needed if buffer will be changed
		HRESULT CreateIndexBuffer(ID3D11Device*& device, T* indexData, UINT numIndices, ID3D11DeviceContext* dc = NULL, unsigned char flags = 0x0)
		{
			if (flags & eBufferFlags_IgnoreCreateTwice && m_buffer)
				m_buffer->Release();
			else
				THROW_POPUP_ERROR((m_buffer == nullptr), "Buffer created twice");

			m_type = eINDEX;
			m_usage = eIMMULATBLE;
			//if (m_buffer)
			//{
			//	Popup::Error("Buffer created twice");
			//	throw;
			//}
			//else if (sizeof(T) != sizeof(DWORD))
			//{
			//	Popup::Error("Indexbuffer type must be DWORD");
			//	throw;
			//}

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

		//HRESULT CreateBuffer(BufferType type, ID3D11Device*& device, T* data, UINT size, ID3D11DeviceContext* dc = NULL)
		//{
		//	m_type = type;
		//	m_bufferSize = size;
		//	mp_dc = dc;
		//	m_usage = eIMMULATBLE;
		//	if (m_buffer)
		//	{
		//		throw;
		//		Popup::Error("Buffer created twice");
		//	}
		//	UINT cpuFlags = 0;
		//	//m_data = new T[size];
		//	for (int i = 0; i < size; i++)
		//	{
		//		m_data[i] = data[i];
		//	}
		//	if (dc)
		//	{
		//		m_usage = eDYNAMIC;
		//		mp_dc = dc;
		//		cpuFlags = D3D11_CPU_ACCESS_WRITE;
		//	}

		//	//m_data = data;
		//	mp_dc = dc;
		//	D3D11_BUFFER_DESC bufferDesc;
		//	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		//	bufferDesc.Usage = (D3D11_USAGE)m_usage;
		//	bufferDesc.ByteWidth = m_stride * m_bufferSize;
		//	bufferDesc.BindFlags = (UINT)type;
		//	bufferDesc.CPUAccessFlags = cpuFlags;
		//	bufferDesc.MiscFlags = 0;
		//	bufferDesc.Usage = (UINT)m_usage;

		//	D3D11_SUBRESOURCE_DATA bufferData;
		//	ZeroMemory(&bufferData, sizeof(D3D11_SUBRESOURCE_DATA));
		//	bufferData.pSysMem = data;

		//	return HRESULT(device->CreateBuffer(&bufferDesc, &bufferData, &m_buffer));
		//}

		void MapBuffer()
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
		ID3D11Device* mp_device = nullptr;
		UINT m_stride = sizeof(T);
		UINT m_bufferSize = 0;
		BufferUsage m_usage = eDEFAULT;
		BufferType m_type = BufferType(0);
		uchar m_flags = 0;
		ID3D11ShaderResourceView* m_srv = nullptr;
	};
}



