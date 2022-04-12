#pragma once
#include <d3d11.h>
class Texture
{
private:
	ID3D11ShaderResourceView* m_textureSRV = nullptr;
public:
	Texture();
	~Texture();
	bool CreateTexture(const char* texturePath, ID3D11Device* device, const bool& flipUV = true);

	ID3D11ShaderResourceView* getTextureResourceView();
	ID3D11ShaderResourceView** getTextureResourceViewAdress();
};