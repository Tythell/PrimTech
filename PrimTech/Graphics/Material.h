#pragma once
#include "Texture.h"
#include "ResourceHandler.h"
#include "../Utility/StringHelper.h"
#include "Buffer.h"
#include <string>
class Material
{
public:
	Material();
	
	void SetPointers(Buffer<hlsl::cbpMaterialBuffer>* cbMaterialBuffer);
	void LoadDiffuse(std::string path);
	void LoadDistortion(std::string path);
	void UpdateTextureScroll(const float& deltatime);
	void SetScrollSpeed(float x, float y);
	void Set(ID3D11DeviceContext*& dc);
private:
	TextureMap* mp_diffuse = nullptr;
	TextureMap* mp_distortion = nullptr;
	sm::Vector2 m_offsetValue;
	sm::Vector2 m_offsetSpeed;
	float m_transparency = 1.f;

	//Buffer<hlsl::cbpWorldTransforms3D>* mp_cbTransformBuffer = nullptr;
	Buffer<hlsl::cbpMaterialBuffer>* mp_matBuffer = nullptr;
};