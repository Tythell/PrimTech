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
		uchar* m_pImageData = nullptr;
		ID3D11Texture2D* m_texture2D = nullptr;
		uint2 m_dimensions = uint2(0);
		uint m_channels = 4;
		ID3D11DeviceContext* m_pdc = nullptr;
	public:
		TextureMap();
		TextureMap(const char* texturePath, ID3D11Device* device, const bool& flipUV = true);
		~TextureMap();
		bool CreateDynamicTexture(unsigned char* imageData, uint2 dimensions, ID3D11Device* device, ID3D11DeviceContext*& dc);
		bool CreateDynamicTexture(const char* path, ID3D11Device* device, ID3D11DeviceContext*& dc);
		bool CreateFromFile(const char* texturePath, ID3D11Device* device, const bool& flipUV = true);
		bool CreateFromData(unsigned char* imageData, ID3D11Device*& m_device, uint2 dimensions, unsigned channels = 4);
		// Only works on dynamic textures
		void SetPixelColor(uint2 pixel, uint color);
		void SetPixelColor(uint2 pixel, float4 color);
		void Update();
		
		bool CreatePerlinNoise(ID3D11Device*& device);

		uchar* GetImageDataP() const { return m_pImageData; };
		ID3D11ShaderResourceView* GetSRV();
		ID3D11ShaderResourceView** GetSRVAdress();
		std::string GetName() const;
	};
}
