#pragma once
#include "Texture.h"
#include "ResourceHandler.h"
#include "../Utility/StringHelper.h"
#include "Buffer.h"
#include <string>
#include "../Macros/Colors.h"

enum TextureType
{
	eDiffuse, eDistortion, eNormal, eOpacity, eTextureTypeAMOUNT
};

enum class eMaterialHeaders
{
	eNull,
	eDIFFUSE, eNORMAL, eDISTORTION, eOPACITY, eTILING, eDIRLIGHT, ePOINTLIGHT,
	eDIFFCLR,
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
	Material(){};
	Material(std::string name);
	void LoadTexture(std::string textureName, TextureType type);
	void UpdateTextureScroll(const float& deltatime);
	void SetDiffuseScrollSpeed(float x, float y);
	void SetDistortionScrollSpeed(float x, float y);
	void Set(ID3D11DeviceContext*& dc);
	void Set(ID3D11DeviceContext*& dc, Buffer<hlsl::cbpMaterialBuffer>& mp_matBuffer);
	void SetTransparency(float f);
	void SetTextureScale(float f);
	void SetTextureScaleDist(float f);
	void SetSelection(bool b);
	void SetRimColor(sm::Vector3 rgb);
	void ResetScrollValue();
	void SetDistortionDivider(const float& f);
	float GetDistortionDivider() const;
	float GetTransparancy() const;
	void SetLeftHanded(bool b);
	bool ExportMaterial(std::string path);
	void ImportMaterial(std::string path);
	void RemoveTexture(const TextureType e);
	void SetDiffuseClr(const sm::Vector3& v);
	void SetDiffuseClr(float r, float g, float b);
	void SetName(std::string name);

	//std::string GetMaterialName() const;
	std::string GetMapName(const TextureType& e) const;
	std::string GetFileName() const;
	sm::Vector2 GetDiffuseScrollSpeed() const;
	sm::Vector2 GetDistortionScrollSpeed() const;
	float GetTextureScale() const;
	float GetTextureScaleDist() const;
	sm::Vector3 GetDiffuseClr() const;

	Buffer<hlsl::cbpMaterialBuffer>* GetBuffer();
	bool HasTexture(const TextureType& e) const;
	bool HasTexture(const UINT& e) const;
	void SetPointers(Buffer<hlsl::cbpMaterialBuffer>* cbMaterialBuffer);
private:
	void ReadRecursion(eMaterialHeaders& header, std::ifstream& reader);
	void ClearMaterial();
	std::string m_name;
	TextureMap* mp_textures[eTextureTypeAMOUNT] = { nullptr };
	float m_textureScale = 1.f;
	float m_textureScaleDist = 1.f;
	bool m_selection = false;
	sm::Vector3 m_rimColor = GOLD_3F;
	float m_distDivider = 1.f;
	bool m_lefthanded = true;
	sm::Vector3 m_diffuseClr = WHITE_3F;

	sm::Vector2 m_diffuseOffsetValue, m_distortionValue;
	sm::Vector2 m_diffuseOffsetSpeed, m_distortionOffsetSpeed;
	float m_transparency = 1.f;

	Buffer<hlsl::cbpMaterialBuffer>* mp_matBuffer;
};