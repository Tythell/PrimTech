#pragma once
#include "Texture.h"
#include "ResourceHandler.h"
#include "../Utility/StringHelper.h"
#include "Buffer.h"
#include <string>
#include "../Colors.h"

enum TextureType
{
	eDiffuse, eDistortion, eNormal, eTextureTypeAMOUNT
};

enum class eMaterialHeaders
{
	eNull,
	eDIFFUSE, eNORMAL, eDISTORTION, eOPACITY, eTILING,
};

//struct MaterialHeader
//{
//	unsigned char diffuseName[16] = {};
//	unsigned char normalName[16] = {};
//	unsigned char distortionName[16] = {};
//	unsigned char opacityMapName[16] = {};
//
//	hlsl::float2 diffSpeed;
//	hlsl::float2 distSpeed;
//	int distDivider;
//	int tiling;
//};
#define FILENAME_MAXSIZE 32
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
	void SetDistortionDivider(const int n);
	int GetDistortionDivider() const;
	float GetTransparancy() const;
	void SetLeftHanded(bool b);
	bool ExportMaterial(std::string path);
	void ImportMaterial(std::string path);
	void RemoveTexture(const TextureType e);

	std::string GetMapName(const TextureType& e) const;
	std::string GetFileName() const;
	sm::Vector2 GetDiffuseScrollSpeed() const;
	sm::Vector2 GetDistortionScrollSpeed() const;
	float GetTextureScale() const;

	bool HasTexture(const TextureType e) const;
private:
	//std::string GetDiffuseName() const;
	//std::string GetDistortionMapName() const;
	//std::string GetNormalMapName() const;

	void ReadRecursion(eMaterialHeaders& header, std::ifstream& reader);
	void LoadDiffuse(std::string path);
	void LoadDistortion(std::string path);
	void LoadNormalMap(std::string path);
	TextureMap* mp_textures[eTextureTypeAMOUNT] = { nullptr };
	//TextureMap* mp_diffuse = nullptr;
	//TextureMap* mp_distortion = nullptr;
	//TextureMap* mp_normalMap = nullptr;
	float m_textureScale = 1.f;
	bool m_selection = false;
	sm::Vector3 m_rimColor = GOLD_3F;
	float m_distDivider = 1.f;
	bool m_lefthanded = true;
	std::string m_name;

	sm::Vector2 m_diffuseOffsetValue, m_distortionValue;
	sm::Vector2 m_diffuseOffsetSpeed, m_distortionOffsetSpeed;
	float m_transparency = 1.f;

	Buffer<hlsl::cbpMaterialBuffer>* mp_matBuffer;
};