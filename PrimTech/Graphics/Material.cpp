#include "Material.h"

void Material::SetPointers(Buffer<hlsl::cbpMaterialBuffer>* cbMaterialBuffer)
{
	mp_matBuffer = cbMaterialBuffer;
}

void Material::LoadTexture(std::string textureName, TextureType type)
{
	switch (type)
	{
	case eDiffuse:
		LoadDiffuse(textureName);
		break;
	case eDistortion:
		LoadDistortion(textureName);
		break;
	case eNormal:
		Popup::Error("Normal map not supported");
		break;
	default:
	{
		std::string s = "Texture type " + std::to_string(type) + " does not exist";
		Popup::Error(s);
		break;
	}
	}
}

void Material::LoadDiffuse(std::string path)
{
	int textureIndex = ResourceHandler::CheckTextureNameExists(StringHelper::GetName(path));
	if (textureIndex != -1)
		mp_diffuse = ResourceHandler::GetTextureAdress(textureIndex);
	else
		mp_diffuse = ResourceHandler::AddTexture(path);
}

void Material::LoadDistortion(std::string path)
{
	int textureIndex = ResourceHandler::CheckTextureNameExists(StringHelper::GetName(path));
	if (textureIndex != -1)
		mp_distortion = ResourceHandler::GetTextureAdress(textureIndex);
	else
		mp_distortion = ResourceHandler::AddTexture(path);
	m_transparency = .8f; // temporary
}

void Material::UpdateTextureScroll(const float& deltatime)
{
	m_diffuseOffsetValue += m_diffuseOffsetSpeed * deltatime;
	m_diffuseOffsetValue.x = fmodf(m_diffuseOffsetValue.x, 1.f);
	m_diffuseOffsetValue.y = fmodf(m_diffuseOffsetValue.y, 1.f);
	if (m_diffuseOffsetValue.x < 0.f) m_diffuseOffsetValue.x += 1.f;
	if (m_diffuseOffsetValue.y < 0.f) m_diffuseOffsetValue.y += 1.f;


	m_distortionValue += m_distortionOffsetSpeed * deltatime;
	m_distortionValue.x = fmodf(m_distortionValue.x, 1.f);
	m_distortionValue.y = fmodf(m_distortionValue.y, 1.f);
	if (m_distortionValue.x < 0.f) m_distortionValue.x += 1.f;
	if (m_distortionValue.y < 0.f) m_distortionValue.y += 1.f;
	//m_distortionValue += m_distortionOffsetSpeed * deltatime;
	//m_distortionValue.x = fmodf(m_distortionValue.x, 1.f);
	//m_distortionValue.y = fmodf(m_distortionValue.y, 1.f);
	//if (m_distortionValue.x < 0.f) m_distortionValue.x += 1.f;
	//if (m_distortionValue.y < 0.f) m_distortionValue.y += 1.f;
}

void Material::SetDiffuseScrollSpeed(float x, float y)
{
	m_diffuseOffsetSpeed = sm::Vector2(x, y);
}

void Material::SetDistortionScrollSpeed(float x, float y)
{
	m_distortionOffsetSpeed = sm::Vector2(x, y);
}

void Material::Set(ID3D11DeviceContext*& dc)
{
	if (mp_diffuse)
		dc->PSSetShaderResources(0, 1, mp_diffuse->GetSRVAdress());
	else // If Model has no diffuse it will default to first texture in vector
		dc->PSSetShaderResources(0, 1, ResourceHandler::GetTextureAdress(0)->GetSRVAdress());

	bool hasDistortion = (mp_distortion != nullptr);

	if(hasDistortion)
		dc->PSSetShaderResources(1, 1, mp_distortion->GetSRVAdress());
	dc->PSSetConstantBuffers(1, 1, mp_matBuffer->GetReference());
	mp_matBuffer->Data().texCoordOffset = m_diffuseOffsetValue;
	mp_matBuffer->Data().texCoordoffsetDist = m_distortionValue;
	mp_matBuffer->Data().hasDistortion = int(hasDistortion);
	mp_matBuffer->Data().transparency = m_transparency;
	mp_matBuffer->UpdateCB();
}

void Material::SetTransparency(float f)
{
	m_transparency = f;
}
