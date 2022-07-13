#pragma once
#include "Texture.h"
#include "ResourceHandler.h"
#include "../Utility/StringHelper.h"
#include "Buffer.h"
#include <string>
#include "../Colors.h"

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
	void SetSelection(bool b);
	void SetRimColor(sm::Vector3 rgb);
	void ResetScrollValue();
	void EnableDistortion(bool b);
	void SetDistortionDivider(int n);
	int GetDistortionDivider() const;
	bool HasDistortion() const;
	float GetTransparancy() const;
	
	sm::Vector2 GetDiffuseScrollSpeed() const;
	sm::Vector2 GetDistortionScrollSpeed() const;
	float GetTextureScale() const;
private:
	void LoadDiffuse(std::string path);
	void LoadDistortion(std::string path);
	void LoadNormalMap(std::string path);
	TextureMap* mp_diffuse = nullptr;
	TextureMap* mp_distortion = nullptr;
	TextureMap* mp_normalMap = nullptr;
	float m_textureScale = 1.f;
	bool m_selection = false;
	sm::Vector3 m_rimColor = GOLD_3F;
	float m_distDivider = 0.f;

	sm::Vector2 m_diffuseOffsetValue, m_distortionValue;
	sm::Vector2 m_diffuseOffsetSpeed, m_distortionOffsetSpeed;
	float m_transparency = 1.f;

	Buffer<hlsl::cbpMaterialBuffer>* mp_matBuffer;
};