#pragma once
#include "Texture.h"
#include "ResourceHandler.h"
#include "../Utility/StringHelper.h"
#include "../Buffer.h"
#include <string>
#include "../Macros/Colors.h"

namespace pt
{
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
		Material() {};
		Material(std::string name);
		//Material(const Material& other);
		void LoadTexture(std::string textureName, TextureType type);
		void UpdateTextureScroll(const float& deltatime);
		void SetDiffuseScrollSpeed(float x, float y);
		void SetDistortionScrollSpeed(float x, float y);
		void Set(ID3D11DeviceContext*& dc, PrimtTech::Buffer<PrimtTech::hlsl::cbpMaterialBuffer>& mp_matBuffer);
		void SetTransparency(float f);
		void SetTextureScale(float f);
		void SetTextureScaleDist(float f);
		void SetRimColor(float3 rgb);
		void ResetScrollValue();
		void SetDistortionDivider(const float& f);
		float GetDistortionDivider() const;
		float GetTransparancy() const;
		
		bool ExportMaterial(std::string path);
		void ImportMaterial(std::string path);
		void RemoveTexture(const TextureType e);
		void SetDiffuseClr(const float3& v);
		void SetDiffuseClr(float r, float g, float b);
		void SetName(std::string name);

		std::string GetMapName(const TextureType& e) const;
		std::string GetFileName() const;
		float2 GetDiffuseScrollSpeed() const;
		float2 GetDistortionScrollSpeed() const;
		float GetTextureScale() const;
		float GetTextureScaleDist() const;
		float3 GetDiffuseClr() const;

		bool HasTexture(const TextureType& e) const;
		bool HasTexture(UINT e) const;
		PrimtTech::TextureMap* GetTexture(uint type);
	private:
		void ReadRecursion(eMaterialHeaders& header, std::ifstream& reader);
		void ClearMaterial();
		std::string m_name;
		PrimtTech::TextureMap* mp_textures[eTextureTypeAMOUNT] = { nullptr };
		float m_textureScale = 1.f;
		float m_textureScaleDist = 1.f;
		
		//float3 m_rimColor = GOLD_3F;
		float m_distDivider = 1.f;
		float3 m_diffuseClr = WHITE_3F;

		float2 m_diffuseOffsetValue, m_distortionValue;
		float2 m_diffuseOffsetSpeed, m_distortionOffsetSpeed;
		float m_transparency = 1.f;
	};
}

