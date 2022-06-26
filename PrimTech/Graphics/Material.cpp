#include "Material.h"

Material::Material()
{
}

void Material::SetPointers(Buffer<hlsl::cbpMaterialBuffer>* cbMaterialBuffer)
{
	mp_matBuffer = cbMaterialBuffer;
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
	m_transparency = .8f;
}

void Material::UpdateTextureScroll(const float& deltatime)
{
	m_offsetValue += m_offsetSpeed * deltatime;
	m_offsetValue.x = fmodf(m_offsetValue.x, 1.f);
	m_offsetValue.y = fmodf(m_offsetValue.y, 1.f);
	if (m_offsetValue.x < 0.f) m_offsetValue.x += 1.f;
	if (m_offsetValue.y < 0.f) m_offsetValue.y += 1.f;
}

void Material::SetScrollSpeed(float x, float y)
{
	m_offsetSpeed.x = x;
	m_offsetSpeed.y = y;
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
	//else
		//dc->PSSetShaderResources(1, 1, NULL);
	mp_matBuffer->Data().texCoordOffset = m_offsetValue;
	mp_matBuffer->Data().hasDistortion = int(hasDistortion);
	mp_matBuffer->Data().transparancy = m_transparency;
	mp_matBuffer->UpdateCB();
}