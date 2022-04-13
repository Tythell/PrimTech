#pragma once
#include <d3d11.h>
class TextureMap
{
private:
	ID3D11ShaderResourceView* m_textureSRV = nullptr;
	bool m_isLoaded = false;
public:
	TextureMap();
	~TextureMap();
	bool CreateTextureFromFile(const char* texturePath, ID3D11Device* device, const bool& flipUV = true);
	bool CreatePerlinNoise(ID3D11Device*& device);

	ID3D11ShaderResourceView* getTextureResourceView();
	ID3D11ShaderResourceView** getTextureResourceViewAdress();
};