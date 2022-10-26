#pragma once
#include <d3d11.h>
#include "../Math/Math.h"
#include "Utility/Popup.h"
enum class ShaderType
{
	VS, GS, HS, DS, PS
};
class TextureMap
{
private:
	ID3D11ShaderResourceView* m_textureSRV = nullptr;
	std::string m_name = "";
	std::string m_fullName = "";
public:
	TextureMap();
	TextureMap(const char* texturePath, ID3D11Device* device, const bool& flipUV = true);
	~TextureMap();
	bool CreateFromFile(const char* texturePath, ID3D11Device* device, const bool& flipUV = true);
	bool CreateFromData(unsigned char* imageData, ID3D11Device*& m_device, d::XMINT2 dimensions, unsigned channels = 4);
	static const unsigned char* CreateCharFromFile(const char* path, unsigned char*& imagedData, int imagewidth, int imageheight, int channels, bool flipUV = true);
	static void ExportCharToImage(const char* path, unsigned char* imageData, int width, int height, int channels);
	bool CreatePerlinNoise(ID3D11Device*& device);
	void Bind(uint slot, ShaderType shadertype, ID3D11DeviceContext*& dc);

	ID3D11ShaderResourceView* GetSRV();
	ID3D11ShaderResourceView** GetSRVAdress();
	std::string GetName() const;
	std::string GetFullName() const;
};