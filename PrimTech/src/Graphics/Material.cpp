#include"pch.h"
#include "Material.h"
#include "Model.h"

namespace PrimtTech
{
	Material::Material(std::string name)
	{
		SetName(name);
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
			reader.read((char*)&m_textureScaleDist, 4);
			//if(std::string(charBuffer) != "")
			LoadTexture(std::string(charBuffer), eDistortion);
			break;
		case eMaterialHeaders::eOPACITY:
			reader.read(charBuffer, FILENAME_MAXSIZE);
			LoadTexture(std::string(charBuffer), eOpacity);
			//reader.read((char*)&m_transparency, 4);
			break;
		case eMaterialHeaders::eTILING:
			reader.read((char*)&m_textureScale, 4);
			break;
		case eMaterialHeaders::eDIFFCLR:
			reader.read((char*)&m_diffuseClr, sizeof(sm::Vector3));
			break;
		default:
			return;
		}
		reader.read((char*)&header, 4);
		ReadRecursion(header, reader);
	}

	sm::Vector3 Material::GetDiffuseClr() const
	{
		return m_diffuseClr;
	}

	void Material::ClearMaterial()
	{
		for (int i = 0; i < eTextureTypeAMOUNT; i++)
		{
			mp_textures[i] = nullptr;
		}
		m_textureScale = 1.f;
		m_textureScaleDist = 1.f;
		m_distDivider = 1.f;
		m_name = "";
		m_transparency = 1.f;
		m_diffuseOffsetValue = { 0.f, 0.f }; m_distortionValue = { 0.f, 0.f };
		m_diffuseOffsetSpeed = { 0.f, 0.f }; m_distortionOffsetSpeed = { 0.f, 0.f };
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
		Set(dc, *mp_matBuffer);
	}

	void Material::Set(ID3D11DeviceContext*& dc, Buffer<hlsl::cbpMaterialBuffer>& matBuffer)
	{
		if (mp_textures[eDiffuse])
			dc->PSSetShaderResources(1, 1, mp_textures[eDiffuse]->GetSRVAdress());
		else // If Model has no diffuse it will default to first texture in vector
			dc->PSSetShaderResources(1, 1, ResourceHandler::GetTextureAdress(0)->GetSRVAdress());

		for (int i = 1; i < eTextureTypeAMOUNT; i++)
		{
			if (HasTexture(TextureType(i)))
				dc->PSSetShaderResources(1 + i, 1, mp_textures[i]->GetSRVAdress());
		}

		matBuffer.Data().textureScaleDist = m_textureScaleDist;

		matBuffer.Data().diffuseColor = m_diffuseClr;

		//dc->PSSetConstantBuffers(1, 1, mp_matBuffer->GetReference());
		matBuffer.Data().texCoordOffset = m_diffuseOffsetValue;

		matBuffer.Data().texCoordoffsetDist = m_distortionValue;
		matBuffer.Data().distDiv = m_distDivider;
		matBuffer.Data().transparency = m_transparency;
		matBuffer.Data().textureScale = m_textureScale;
		matBuffer.Data().rimColor = m_rimColor;
		matBuffer.Data().rim = (int)m_selection;

		


		for (UINT i = 0; i < eTextureTypeAMOUNT; i++)
		{
			UINT bitFlag = (1 << i);
			if (HasTexture(i))
				matBuffer.Data().flags |= bitFlag;
			else
				matBuffer.Data().flags &= ~bitFlag;
		}
		matBuffer.MapBuffer();
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

	void Material::SetDistortionDivider(const float& f)
	{
		m_distDivider = f;
	}

	float Material::GetDistortionDivider() const
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

		if (HasTexture(eDiffuse))
		{
			header = eMaterialHeaders::eDIFFUSE;
			writer.write((const char*)&header, sizeof(eMaterialHeaders));
			writer.write((const char*)mp_textures[eDiffuse]->GetName().c_str(), FILENAME_MAXSIZE);
			writer.write((const char*)&m_diffuseOffsetSpeed, sizeof(sm::Vector2));
		}
		else
		{
			header = eMaterialHeaders::eDIFFCLR;
			writer.write((const char*)&header, sizeof(eMaterialHeaders));
			writer.write((const char*)&m_diffuseClr, sizeof(sm::Vector3));
		}
		writer.write((const char*)&m_transparency, sizeof(float));
		if (HasTexture(eNormal))
		{
			header = eMaterialHeaders::eNORMAL;
			writer.write((const char*)&header, sizeof(eMaterialHeaders));
			writer.write((const char*)mp_textures[eNormal]->GetName().c_str(), FILENAME_MAXSIZE);
		}
		if (HasTexture(eDistortion))
		{
			header = eMaterialHeaders::eDISTORTION;
			writer.write((const char*)&header, sizeof(eMaterialHeaders));
			writer.write((const char*)mp_textures[eDistortion]->GetName().c_str(), FILENAME_MAXSIZE);
			writer.write((const char*)&m_distortionOffsetSpeed, sizeof(sm::Vector2));
			writer.write((const char*)&m_distDivider, sizeof(float));
			writer.write((const char*)&m_textureScaleDist, sizeof(float));
		}
		if (HasTexture(eOpacity))
		{
			header = eMaterialHeaders::eOPACITY;
			writer.write((const char*)&header, sizeof(eMaterialHeaders));
			writer.write((const char*)mp_textures[eOpacity]->GetName().c_str(), FILENAME_MAXSIZE);
			//writer.write((const char*)&m_transparency, 4);
		}
		if (m_textureScale != 1.f)
		{
			header = eMaterialHeaders::eTILING;
			writer.write((const char*)&header, sizeof(eMaterialHeaders));
			writer.write((const char*)&m_textureScale, 4);
		}
		header = eMaterialHeaders::eNull;
		writer.write((const char*)&header, 4);
		writer.close();
		return true;
	}

	void Material::ImportMaterial(std::string path)
	{
		ClearMaterial();
		if (StringHelper::GetExtension(path) == "pmtrl")
		{
			path = "Assets/pmtrl/" + path;
		}
		else if (StringHelper::GetExtension(path) == "mtl")
		{
			path = "Assets/models/" + path;

			std::ifstream matreader(path);
			if (matreader.is_open())
			{
				int nofMats = 0;
				std::string sdummy;
				std::vector<Mtl> localMtls;
				while (std::getline(matreader, sdummy))
				{
					std::string input;
					if (sdummy[0] == 'n' && sdummy[1] == 'e')
					{
						Mtl mtl;
						input = input = sdummy.substr(7);
						nofMats++;
						mtl.name = input;
						//mtls.emplace_back(mtl);
						localMtls.resize(localMtls.size() + 1);
						localMtls[localMtls.size() - 1] = mtl;
					}
					matreader >> input;
					if (input == "newmtl")
					{
						Mtl mtl;
						nofMats++;
						matreader >> input;
						mtl.name = input;
						localMtls.resize(localMtls.size() + 1);
						localMtls[localMtls.size() - 1] = mtl;
						//mtls.emplace_back(mtl);
					}
					else if (input == "map_Kd")
					{
						matreader >> input;
						localMtls[nofMats - 1].diffuseName = input;
					}

				}
				matreader.close();
				for (int i = 0; i < localMtls.size(); i++)
				{
					if (!localMtls[i].diffuseName.empty())
					{
						LoadTexture(localMtls[i].diffuseName, eDiffuse);

					}
				}
			}
		}
		else
			return;


		m_name = path;

		std::string err = "Failed to open: " + path;

		std::ifstream reader(path.c_str(), std::ios::binary | std::ios::in);
		POPUP_MESSAGE(reader.is_open(), ("Failed to open " + m_name));

		eMaterialHeaders header = eMaterialHeaders::eNull;
		reader.read((char*)&header, 4);

		ReadRecursion(header, reader);
		reader.close();
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

	void Material::SetDiffuseClr(const sm::Vector3& v)
	{
		m_diffuseClr = v;
	}

	std::string Material::GetFileName() const
	{
		return m_name;
	}

	void Material::SetDiffuseClr(float r, float g, float b)
	{
		SetDiffuseClr(sm::Vector3(r, g, b));
	}

	void Material::SetName(std::string name)
	{
		m_name = name;
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

	Buffer<hlsl::cbpMaterialBuffer>* Material::GetBuffer()
	{
		return mp_matBuffer;
	}

	bool Material::HasTexture(const TextureType& e) const
	{
		return (mp_textures[e] != nullptr);
	}
	bool Material::HasTexture(const UINT& e) const
	{
		return (mp_textures[e] != nullptr);
	}

	void Material::SetPointers(Buffer<hlsl::cbpMaterialBuffer>* cbMaterialBuffer)
	{
		mp_matBuffer = cbMaterialBuffer;
	}
}

