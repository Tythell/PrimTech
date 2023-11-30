#include"pch.h"
//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "../3rdParty/stb_image.h"
//#include "../3rdParty/stb_image_write.h"
#include "Texture.h"
#include "LoaderFuncs.h"
#include"../Utility/Popup.h"
#include "ResourceHandler.h"

namespace PrimtTech
{
	TextureMap::TextureMap()
	{}

	TextureMap::TextureMap(const char* texturePath, ID3D11Device* device, const bool& flipUV)
	{
		CreateFromFile(texturePath, device, flipUV);
	}

	TextureMap::~TextureMap()
	{
		if (m_textureSRV)
			m_textureSRV->Release();
		if (m_texture2D)
			m_texture2D->Release();
		if (m_pImageData)
		{
			free(m_pImageData);
		}
		
	}

	bool TextureMap::CreateDynamicTexture(unsigned char* imageData, uint2 dimensions, ID3D11Device* device, ID3D11DeviceContext*& dc)
	{
		m_pdc = dc;
		if (m_textureSRV)
			m_textureSRV->Release();

		m_pImageData = imageData;

		m_dimensions = dimensions;

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = dimensions.x;
		desc.Height = dimensions.y;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data = {};

		data.pSysMem = &m_pImageData[0];
		data.SysMemPitch = dimensions.x * 4;
		data.SysMemSlicePitch = 0;


		if (FAILED(device->CreateTexture2D(&desc, &data, &m_texture2D)))
		{
			return false;
		}

		HRESULT hr = device->CreateShaderResourceView(m_texture2D, nullptr, &m_textureSRV);
		
		return SUCCEEDED(hr);
		//return true;
	}

	bool TextureMap::CreateDynamicTexture(const char* path, ID3D11Device* device, ID3D11DeviceContext*& dc)
	{
		m_pdc = dc;
		if (m_textureSRV)
			m_textureSRV->Release();

		int2 dimen;

		if (!FileLoader::StbiCreateCharFromFile(path, m_pImageData, dimen.x, dimen.y, 4))
		{
			m_pImageData = nullptr;
			return false;
		}

		m_dimensions = (uint2)dimen;

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = m_dimensions.x;
		desc.Height = m_dimensions.y;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data = {};

		data.pSysMem = &m_pImageData[0];
		data.SysMemPitch = m_dimensions.x * 4;
		data.SysMemSlicePitch = 0;


		if (FAILED(device->CreateTexture2D(&desc, &data, &m_texture2D)))
		{
			return false;
		}

		HRESULT hr = device->CreateShaderResourceView(m_texture2D, nullptr, &m_textureSRV);

		return SUCCEEDED(hr);
		//return true;
	}

	bool TextureMap::CreateFromFile(const char* texturePath, ID3D11Device* device, const bool& flipUV)
	{
		if (m_textureSRV)
			m_textureSRV->Release();

		//stbi_set_flip_vertically_on_load(flipUV);

		m_name = StringHelper::GetName(std::string(texturePath));

		std::string fullpath;
		if (texturePath[0] == '.')
			fullpath = std::string(texturePath).substr(1);
		else if (texturePath[1] != ':')
			fullpath = "Assets/Textures/" + std::string(texturePath);
		else
			fullpath = "Assets\\Textures\\" + m_name;


		int textureWidth = 0;
		int textureHeight = 0;
		//unsigned char* imageData = stbi_load(fullpath.c_str(), &textureWidth, &textureHeight, nullptr, STBI_rgb_alpha);
		//unsigned char* imageData = nullptr;
		FileLoader::StbiCreateCharFromFile(fullpath.c_str(), m_pImageData, textureWidth, textureHeight, 4);
		m_channels = 4;

		m_dimensions.x = textureWidth;
		m_dimensions.y = textureHeight;

		if (!m_pImageData)
		{
			Popup::Error("Texture: " + std::string(fullpath) + " not found");
			return false;
		}

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = textureWidth;
		desc.Height = textureHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data = {};

		data.pSysMem = &m_pImageData[0];
		data.SysMemPitch = textureWidth * 4;
		data.SysMemSlicePitch = 0;


		if (FAILED(device->CreateTexture2D(&desc, &data, &m_texture2D)))
		{
			return false;
		}

		HRESULT hr = device->CreateShaderResourceView(m_texture2D, nullptr, &m_textureSRV);

		m_texture2D->Release();
		m_texture2D = nullptr;

		free(m_pImageData);
		m_pImageData = nullptr;
		return SUCCEEDED(hr);
	}

	bool TextureMap::CreateFromData(unsigned char* imageData, ID3D11Device*& m_device, uint2 dimensions, unsigned channels)
	{
		m_channels = 4;
		if (m_textureSRV)
			m_textureSRV->Release();

		m_dimensions = dimensions;

		ID3D11Texture2D* texture = nullptr;
		D3D11_TEXTURE2D_DESC desc = {};

		desc.Width = dimensions.x;
		desc.Height = dimensions.y;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		switch (channels)
		{
		case 1:
			desc.Format = DXGI_FORMAT_R8_UNORM;
			channels = 1;
			break;
		default:
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			channels = 4;
			break;
		}
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;


		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = &imageData[0];
		data.SysMemPitch = dimensions.x * channels;
		data.SysMemSlicePitch = 0;

		HRESULT hr = m_device->CreateTexture2D(&desc, &data, &texture);
		if (FAILED(hr))
		{
			throw;
		}
		//COM_ERROR(hr, "Init texture from data failed");
		texture->Release();

		return true;
	}

	void TextureMap::SetPixelColor(uint2 pixel, uint color)
	{
		pixel.y = m_dimensions.y - pixel.y - 1;
		uint index = (pixel.y * m_dimensions.x + pixel.x) * 4;
		memcpy(&m_pImageData[index], &color, sizeof(uint));
	}

	void TextureMap::SetPixelColor(uint2 pixel, float4 color)
	{
		SetPixelColor(pixel, pt::Color::AsHex(color));
	}

	void TextureMap::Update()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = m_pdc->Map(m_texture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		COM_ERROR(hr, "Failed to update texture");
		CopyMemory(mappedResource.pData, m_pImageData, sizeof(uchar) * m_dimensions.x * m_dimensions.y * 4);
		m_pdc->Unmap(m_texture2D, 0);
	}

	bool TextureMap::CreatePerlinNoise(ID3D11Device*& device)
	{
		return false;
	}

	ID3D11ShaderResourceView* TextureMap::GetSRV()
	{
		return m_textureSRV;
	}

	ID3D11ShaderResourceView** TextureMap::GetSRVAdress()
	{
		return &m_textureSRV;
	}

	std::string TextureMap::GetName() const
	{
		return m_name;
	}

}

