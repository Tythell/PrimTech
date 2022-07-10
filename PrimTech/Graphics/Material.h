#pragma once
#include "Texture.h"
#include "ResourceHandler.h"
#include "../Utility/StringHelper.h"
#include "Buffer.h"
#include <string>

enum TextureType
{
	eDiffuse, eDistortion, eNormal, 
};

class Material
{
public:
	void SetPointers(Buffer<hlsl::cbpMaterialBuffer>* cbMaterialBuffer);
	void LoadTexture(std::string textureName, TextureType type);
	void UpdateTextureScroll(const float& deltatime);
	void SetDiffuseScrollSpeed(float x, float y);
	void SetDistortionScrollSpeed(float x, float y);
	void Set(ID3D11DeviceContext*& dc);
	void SetTransparency(float f);
	void SetTextureScale(float f);
	//void Create(ID3D11Device*& d, ID3D11DeviceContext*& dc);
private:
	void LoadDiffuse(std::string path);
	void LoadDistortion(std::string path);
	TextureMap* mp_diffuse = nullptr;
	TextureMap* mp_distortion = nullptr;
	float m_textureScale = 1.f;

	sm::Vector2 m_diffuseOffsetValue, m_distortionValue;
	sm::Vector2 m_diffuseOffsetSpeed, m_distortionOffsetSpeed;
	float m_transparency = 1.f;

	Buffer<hlsl::cbpMaterialBuffer>* mp_matBuffer;
};