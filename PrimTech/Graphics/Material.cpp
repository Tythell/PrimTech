#include "Material.h"
#include <fstream>

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
		LoadNormalMap(textureName);
		break;
	default:
	{
		std::string s = "Texture type " + std::to_string(type) + " does not exist";
		Popup::Error(s);
		break;
	}
	}
}

void Material::ReadRecursion(eMaterialHeaders& header, std::ifstream& reader)
{
	char charBuffer[FILENAME_MAXSIZE] = { "" };
	switch (header)
	{
	case eMaterialHeaders::eDIFFUSE:
		reader.read(charBuffer, FILENAME_MAXSIZE);
		LoadDiffuse(std::string(charBuffer));
		reader.read((char*)&m_diffuseOffsetSpeed, sizeof(sm::Vector2));
		break;
	case eMaterialHeaders::eNORMAL:
		reader.read(charBuffer, FILENAME_MAXSIZE);
		LoadNormalMap(std::string(charBuffer));
		break;
	case eMaterialHeaders::eDISTORTION:
		reader.read(charBuffer, FILENAME_MAXSIZE);
		reader.read((char*)& m_distDivider, 4);
		LoadDistortion(std::string(charBuffer));
		break;
	//case eMaterialHeaders::eOPACITY:
	//	break;
	case eMaterialHeaders::eTILING:
		reader.read((char*)&m_textureScale, 4);
		break;
	default:
		return;
	}
	reader.read((char*)&header, 4);
	ReadRecursion(header, reader);
}

void Material::LoadDiffuse(std::string path)
{
	int textureIndex = ResourceHandler::CheckTextureNameExists(StringHelper::GetName(path));
	if (textureIndex != -1)
		mp_textures[eDiffuse] = ResourceHandler::GetTextureAdress(textureIndex);
	else
		mp_textures[eDiffuse] = ResourceHandler::AddTexture(path);
}

void Material::LoadDistortion(std::string path)
{
	int textureIndex = ResourceHandler::CheckTextureNameExists(StringHelper::GetName(path));
	if (textureIndex != -1)
		mp_textures[eDistortion] = ResourceHandler::GetTextureAdress(textureIndex);
	else
		mp_textures[eDistortion] = ResourceHandler::AddTexture(path);
}

void Material::LoadNormalMap(std::string path)
{
	int textureIndex = ResourceHandler::CheckTextureNameExists(StringHelper::GetName(path));
	if (textureIndex != -1)
		mp_textures[eNormal] = ResourceHandler::GetTextureAdress(textureIndex);
	else
		mp_textures[eNormal] = ResourceHandler::AddTexture(path);
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
	if (mp_textures[eDiffuse])
		dc->PSSetShaderResources(0, 1, mp_textures[eDiffuse]->GetSRVAdress());
	else // If Model has no diffuse it will default to first texture in vector
		dc->PSSetShaderResources(0, 1, ResourceHandler::GetTextureAdress(0)->GetSRVAdress());

	bool hasDistortion = (mp_textures[eDistortion] != nullptr);
	bool hasNormalMap = (mp_textures[eNormal] != nullptr);

	if(hasDistortion)
		dc->PSSetShaderResources(1, 1, mp_textures[eDistortion]->GetSRVAdress());
	if(hasNormalMap)
		dc->PSSetShaderResources(3, 1, mp_textures[eNormal]->GetSRVAdress());
		
	dc->PSSetConstantBuffers(1, 1, mp_matBuffer->GetReference());
	mp_matBuffer->Data().texCoordOffset = m_diffuseOffsetValue;
	mp_matBuffer->Data().LH = (int)m_lefthanded;
	mp_matBuffer->Data().texCoordoffsetDist = m_distortionValue;
	mp_matBuffer->Data().hasDistortion = int(hasDistortion);
	mp_matBuffer->Data().hasNormal = int(hasNormalMap);
	mp_matBuffer->Data().distDiv = m_distDivider;
	mp_matBuffer->Data().transparency = m_transparency;
	mp_matBuffer->Data().textureScale = m_textureScale;
	mp_matBuffer->Data().rimColor = m_rimColor;
	mp_matBuffer->Data().rim = (int)m_selection;
	mp_matBuffer->UpdateCB();
}

void Material::SetTransparency(float f)
{
	m_transparency = f;
}

void Material::SetTextureScale(float f)
{
	m_textureScale = f;
}

void Material::SetSelection(bool b)
{
	m_selection = b;
}

void Material::SetRimColor(sm::Vector3 rgb)
{
	m_rimColor = rgb;
}

void Material::ResetScrollValue()
{
	m_diffuseOffsetValue = sm::Vector2();
	m_distortionValue = sm::Vector2();
}

void Material::SetDistortionDivider(const int n)
{
	m_distDivider = n;
}

int Material::GetDistortionDivider() const
{
	return m_distDivider;
}

float Material::GetTransparancy() const
{
	return m_transparency;
}

void Material::SetLeftHanded(bool b)
{
	m_lefthanded = b;
}

bool Material::ExportMaterial(std::string path)
{
	std::ofstream writer(path, std::ios::binary);
	eMaterialHeaders header;
	if (!writer.is_open())
		Popup::Error("Failed to export material");

	if (mp_textures[eDiffuse] != nullptr)
	{
		header = eMaterialHeaders::eDIFFUSE;
		writer.write((const char*)&header, 4);
		writer.write((const char*)mp_textures[eDiffuse]->GetName().c_str(), FILENAME_MAXSIZE);
		writer.write((const char*)&m_diffuseOffsetSpeed, sizeof(sm::Vector2));
	}
	if (mp_textures[eNormal] != nullptr)
	{
		header = eMaterialHeaders::eNORMAL;
		writer.write((const char*)&header, 4);
		writer.write((const char*)mp_textures[eNormal]->GetName().c_str(), FILENAME_MAXSIZE);
	}
	if (mp_textures[eDistortion] != nullptr)
	{
		header = eMaterialHeaders::eDISTORTION;
		writer.write((const char*)&header, 4);
		writer.write((const char*)mp_textures[eDistortion]->GetName().c_str(), FILENAME_MAXSIZE);
		writer.write((const char*)&m_distDivider, 4);
	}
	if (m_textureScale != 1.f)
	{
		header = eMaterialHeaders::eTILING;
		writer.write((const char*)&header, 4);
		writer.write((const char*)&m_textureScale, 4);
	}
	header = eMaterialHeaders::eNull;
	writer.write((const char*)&header, 4);
	writer.close();
	return true;
}

void Material::ImportMaterial(std::string path)
{
	m_name = path;
	std::string err = "Failed to open: " + path;
	path = "Assets/pmtrl/" + path;
 	std::ifstream reader(path.c_str(), std::ios::binary | std::ios::in);
	if (reader.is_open())
	{
		eMaterialHeaders header = eMaterialHeaders::eNull;
		reader.read((char*)&header, 4);

		ReadRecursion(header, reader);
		reader.close();
	}
	else
	{
		Popup::Error("Failed to open material");
	}
}

void Material::RemoveTexture(const TextureType e)
{
	mp_textures[e] = nullptr;
}

std::string Material::GetMapName(const TextureType& e) const
{
	if (!mp_textures[e]) return "does not exist";
	return mp_textures[e]->GetName();
}

sm::Vector2 Material::GetDiffuseScrollSpeed() const
{
	return m_diffuseOffsetSpeed;
}

sm::Vector2 Material::GetDistortionScrollSpeed() const
{
	return m_distortionOffsetSpeed;
}

float Material::GetTextureScale() const
{
	return m_textureScale;
}

bool Material::HasTexture(const TextureType e) const
{
	return (mp_textures[e] != nullptr);
}