#pragma once
#include <d3d11.h>
//#include <PerlinNoise.hpp>
#include "../Math/Math.h"

namespace PrimtTech
{
	class TextureMap
	{
	private:
		ID3D11ShaderResourceView* m_textureSRV = nullptr;
		std::string m_name = "";
	public:
		TextureMap();
		TextureMap(const char* texturePath, ID3D11Device* device, const bool& flipUV = true);
		~TextureMap();
		bool CreateFromFile(const char* texturePath, ID3D11Device* device, const bool& flipUV = true);
		bool CreateFromData(unsigned char* imageData, ID3D11Device*& m_device, int2 dimensions, unsigned channels = 4);
		
		bool CreatePerlinNoise(ID3D11Device*& device);

		ID3D11ShaderResourceView* GetSRV();
		ID3D11ShaderResourceView** GetSRVAdress();
		std::string GetName() const;
	};
}
