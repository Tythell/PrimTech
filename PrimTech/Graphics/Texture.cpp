#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Texture.h"

Texture::Texture()
{
}

Texture::~Texture()
{
	if (m_textureSRV)
		m_textureSRV->Release();
}

bool Texture::CreateTexture(const char* texturePath, ID3D11Device* device, const bool& flipUV)
{
	stbi_set_flip_vertically_on_load(flipUV);

	int textureWidth;
	int textureHeight;
	unsigned char* imageData = stbi_load(texturePath, &textureWidth, &textureHeight, nullptr, STBI_rgb_alpha);

	if (!imageData)
		imageData = stbi_load("missingTexture.png", &textureWidth, &textureHeight, nullptr, STBI_rgb_alpha);

	ID3D11Texture2D* texture2D;

	D3D11_TEXTURE2D_DESC desc;
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

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &imageData[0];
	data.SysMemPitch = textureWidth * 4;
	data.SysMemSlicePitch = 0;

	stbi_image_free;

	if (FAILED(device->CreateTexture2D(&desc, &data, &texture2D)))
	{
		return false;
	}

	HRESULT hr = device->CreateShaderResourceView(texture2D, nullptr, &m_textureSRV);
	texture2D->Release();

	return SUCCEEDED(hr);
}

ID3D11ShaderResourceView* Texture::getTextureResourceView()
{
	return m_textureSRV;
}

ID3D11ShaderResourceView** Texture::getTextureResourceViewAdress()
{
	return &m_textureSRV;
}
