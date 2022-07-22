#include "Material.h"
#include <fstream>

void Material::SetPointers(Buffer<hlsl::cbpMaterialBuffer>* cbMaterialBuffer)
{
	mp_matBuffer = cbMaterialBuffer;
}

void Material::LoadTexture(std::string textureName, TextureType type)
{
	int textureIndex = ResourceHandler::CheckTextureNameExists(StringHelper::GetName(textureName));
	if (textureIndex != -1)
		mp_textures[type] = ResourceHandler::GetTextureAdress(textureIndex);
	else
		mp_textures[type] = ResourceHandler::AddTexture(textureName);
}

void Material::ReadRecursion(eMaterialHeaders& header, std::ifstream& reader)
{
	char charBuffer[FILENAME_MAXSIZE] = { "" };
	switch (header)
	{
	case eMaterialHeaders::eDIFFUSE:
		reader.read(charBuffer, FILENAME_MAXSIZE);
		LoadTexture(std::string(charBuffer), eDiffuse);
		reader.read((char*)&m_diffuseOffsetSpeed, sizeof(sm::Vector2));
		reader.read((char*)&m_transparency, 4);
		break;
	case eMaterialHeaders::eNORMAL:
		reader.read(charBuffer, FILENAME_MAXSIZE);
		LoadTexture(std::string(charBuffer), eNormal);
		break;
	case eMaterialHeaders::eDISTORTION:
		reader.read(charBuffer, FILENAME_MAXSIZE);
		reader.read((char*)&m_distortionOffsetSpeed, sizeof(sm::Vector2));
		reader.read((char*)&m_distDivider, 4);
		if(std::string(charBuffer) != "")
			LoadTexture(std::string(charBuffer), eDistortion);
		break;
	case eMaterialHeaders::eOPACITY:
		reader.read(charBuffer, FILENAME_MAXSIZE);
		reader.read((char*)&m_transparency, 4);
		break;
	case eMaterialHeaders::eTILING:
		reader.read((char*)&m_textureScale, 4);
		break;
	default:
		return;
	}
	reader.read((char*)&header, 4);
	ReadRecursion(header, reader);
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
		dc->PSSetShaderResources(1, 1, mp_textures[eDiffuse]->GetSRVAdress());
	else // If Model has no diffuse it will default to first texture in vector
		dc->PSSetShaderResources(1, 1, ResourceHandler::GetTextureAdress(0)->GetSRVAdress());

	for (int i = 1; i < eTextureTypeAMOUNT; i++)
	{
		HasTexture(TextureType(i));
		if ((mp_textures[i] != nullptr))
			dc->PSSetShaderResources(1 + i, 1, mp_textures[i]->GetSRVAdress());
	}

	bool hasDistortion = (mp_textures[eDistortion] != nullptr);
	bool hasNormalMap = (mp_textures[eNormal] != nullptr);
	bool hasOpacityMap = (mp_textures[eOpacity] != nullptr);

	mp_matBuffer->Data().textureScaleDist = m_textureScaleDist;
	mp_matBuffer->Data().hasDistortion = int(hasDistortion);
	mp_matBuffer->Data().hasNormal = int(hasNormalMap);
	mp_matBuffer->Data().hasOpacityMap = int(hasOpacityMap);

	//dc->PSSetConstantBuffers(1, 1, mp_matBuffer->GetReference());
	mp_matBuffer->Data().texCoordOffset = m_diffuseOffsetValue;
	mp_matBuffer->Data().LH = (int)m_lefthanded;
	mp_matBuffer->Data().texCoordoffsetDist = m_distortionValue;

	mp_matBuffer->Data().distDiv = m_distDivider;
	mp_matBuffer->Data().transparency = m_transparency;
	mp_matBuffer->Data().textureScale = m_textureScale;
	mp_matBuffer->Data().rimColor = m_rimColor;
	mp_matBuffer->Data().rim = (int)m_selection;
	mp_matBuffer->UpdateBuffer();
}

void Material::SetTransparency(float f)
{
	m_transparency = f;
}

void Material::SetTextureScale(float f)
{
	m_textureScale = f;
}

void Material::SetTextureScaleDist(float f)
{
	m_textureScaleDist = f;
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
	m_name = StringHelper::GetName(path);
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
		writer.write((const char*)&m_transparency, 4);
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
		writer.write((const char*)&m_distortionOffsetSpeed, sizeof(sm::Vector2));
		writer.write((const char*)&m_distDivider, 4);
	}
	if (mp_textures[eOpacity])
	{
		header = eMaterialHeaders::eOPACITY;
		writer.write((const char*)&header, 4);
		writer.write((const char*)mp_textures[eOpacity]->GetName().c_str(), FILENAME_MAXSIZE);
		writer.write((const char*)&m_transparency, 4);
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
		POPUP_ERROR(false, ("Failed to open" + m_name));
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

std::string Material::GetFileName() const
{
	return m_name;
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

float Material::GetTextureScaleDist() const
{
	return m_textureScaleDist;
}

bool Material::HasTexture(const TextureType e) const
{
	return (mp_textures[e] != nullptr);
}