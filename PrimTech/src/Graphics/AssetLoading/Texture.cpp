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
		unsigned char* imageData = nullptr;
		FileLoader::StbiCreateCharFromFile(fullpath.c_str(), imageData, textureWidth, textureHeight, 4);

		if (!imageData)
		{
			Popup::Error("Texture: " + std::string(fullpath) + " not found");
			return false;
		}

		ID3D11Texture2D* texture2D = nullptr;

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

		data.pSysMem = &imageData[0];
		data.SysMemPitch = textureWidth * 4;
		data.SysMemSlicePitch = 0;


		if (FAILED(device->CreateTexture2D(&desc, &data, &texture2D)))
		{
			return false;
		}

		HRESULT hr = device->CreateShaderResourceView(texture2D, nullptr, &m_textureSRV);
		texture2D->Release();

		free(imageData);
		return SUCCEEDED(hr);
	}

	bool TextureMap::CreateFromData(unsigned char* imageData, ID3D11Device*& m_device, d::XMINT2 dimensions, unsigned channels)
	{
		if (m_textureSRV)
			m_textureSRV->Release();

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

	bool TextureMap::CreatePerlinNoise(ID3D11Device*& device)
	{
		//const siv::PerlinNoise::seed_type seed = 7938181;

		//const siv::PerlinNoise perlin{ 7938181 };

		//unsigned char imageData[256 * 256]{};

		//for (int y = 0; y < 256; y++)
		//{
		//	for (int x = 0; x < 256; x++)
		//	{
		//		imageData[x * y] = perlin.octave2D_01((x * 0.01), (y * 0.01), 4);
		//	}
		//}

		//CreateFromData(imageData, device, { 256, 256 }, 1);

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
