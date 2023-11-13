#include"pch.h"
#include "Material.h"
#include "Mesh.h"

namespace pt
{
	Material::Material(std::string name)
	{
		SetName(name);
	}

	//Material::Material(const Material& other)
	//{
	//	m_name =					other.m_name;
	//	m_textureScale =			other.m_textureScale;
	//	m_textureScaleDist =		other.m_textureScaleDist;
	//	m_rimColor =				other.m_rimColor;
	//	m_distDivider =				other.m_distDivider;
	//	m_diffuseClr =				other.m_diffuseClr;
	//	m_diffuseOffsetValue =		other.m_diffuseOffsetValue;
	//	m_distortionValue =			other.m_distortionValue;
	//	m_diffuseOffsetSpeed =		other.m_diffuseOffsetSpeed;
	//	m_distortionOffsetSpeed =	other.m_distortionOffsetSpeed;
	//	m_transparency =			other.m_transparency;

	//	mp_textures = ;
	//}

	void Material::LoadTexture(std::string textureName, TextureType type)
	{
		int textureIndex = PrimtTech::ResourceHandler::CheckTextureNameExists(StringHelper::GetName(textureName));
		if (textureIndex != -1)
			mp_textures[type] = PrimtTech::ResourceHandler::GetTextureAdress(textureIndex);
		else
			mp_textures[type] = PrimtTech::ResourceHandler::AddTexture(textureName);
	}

	void Material::ReadRecursion(eMaterialHeaders& header, std::ifstream& reader)
	{
		char charBuffer[FILENAME_MAXSIZE] = { "" };

		switch (header)
		{
		case eMaterialHeaders::eDIFFUSE:
			reader.read(charBuffer, FILENAME_MAXSIZE);
			LoadTexture(std::string(charBuffer), eDiffuse);
			reader.read((char*)&m_diffuseOffsetSpeed, sizeof(float2));
			reader.read((char*)&m_transparency, 4);
			break;
		case eMaterialHeaders::eNORMAL:
			reader.read(charBuffer, FILENAME_MAXSIZE);
			LoadTexture(std::string(charBuffer), eNormal);
			break;
		case eMaterialHeaders::eDISTORTION:
			reader.read(charBuffer, FILENAME_MAXSIZE);
			reader.read((char*)&m_distortionOffsetSpeed, sizeof(float2));
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
			reader.read((char*)&m_diffuseClr, sizeof(float3));
			break;
		default:
			return;
		}
		reader.read((char*)&header, 4);
		ReadRecursion(header, reader);
	}

	float3 Material::GetDiffuseClr() const
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
		m_diffuseOffsetSpeed = float2(x, y);
	}

	void Material::SetDistortionScrollSpeed(float x, float y)
	{
		m_distortionOffsetSpeed = float2(x, y);
	}

	void Material::Set(ID3D11DeviceContext*& dc, PrimtTech::Buffer<PrimtTech::hlsl::cbpMaterialBuffer>& matBuffer)
	{
		if (mp_textures[eDiffuse])
			dc->PSSetShaderResources(1, 1, mp_textures[eDiffuse]->GetSRVAdress());
		else // If Model has no diffuse it will default to first texture in vector
			dc->PSSetShaderResources(1, 1, PrimtTech::ResourceHandler::GetTextureAdress(0)->GetSRVAdress());

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
		//matBuffer.Data().rimColor = m_rimColor;


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

	void Material::SetRimColor(float3 rgb)
	{
		//m_rimColor = rgb;
	}

	void Material::ResetScrollValue()
	{
		m_diffuseOffsetValue = float2();
		m_distortionValue = float2();
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

	bool Material::ExportMaterial(std::string path)
	{
		//m_name = StringHelper::GetName(path);
		std::ofstream writer(path, std::ios::binary);
		eMaterialHeaders header;
		if (!writer.is_open())
			Popup::Error("Failed to export material");

		writer.write(m_name.c_str(), 32);

		if (HasTexture(eDiffuse))
		{
			header = eMaterialHeaders::eDIFFUSE;
			writer.write((const char*)&header, sizeof(eMaterialHeaders));
			writer.write((const char*)mp_textures[eDiffuse]->GetName().c_str(), FILENAME_MAXSIZE);
			writer.write((const char*)&m_diffuseOffsetSpeed, sizeof(float2));
		}
		else
		{
			header = eMaterialHeaders::eDIFFCLR;
			writer.write((const char*)&header, sizeof(eMaterialHeaders));
			writer.write((const char*)&m_diffuseClr, sizeof(float3));
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
			writer.write((const char*)&m_distortionOffsetSpeed, sizeof(float2));
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
				std::vector<PrimtTech::Mtl> localMtls;
				while (std::getline(matreader, sdummy))
				{
					std::string input;
					if (sdummy[0] == 'n' && sdummy[1] == 'e')
					{
						PrimtTech::Mtl mtl;
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
						PrimtTech::Mtl mtl;
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


		//m_name = path;

		std::string err = "Failed to open: " + path;

		std::ifstream reader(path.c_str(), std::ios::binary | std::ios::in);
		POPUP_MESSAGE(reader.is_open(), ("Failed to open " + m_name));

		eMaterialHeaders header = eMaterialHeaders::eNull;

		char matName[32] = {""};

		reader.read(matName, 32);
		m_name = matName;

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

	void Material::SetDiffuseClr(const float3& v)
	{
		m_diffuseClr = v;
	}

	std::string Material::GetFileName() const
	{
		return m_name;
	}

	void Material::SetDiffuseClr(float r, float g, float b)
	{
		SetDiffuseClr(float3(r, g, b));
	}

	void Material::SetName(std::string name)
	{
		m_name = name;
	}

	float2 Material::GetDiffuseScrollSpeed() const
	{
		return m_diffuseOffsetSpeed;
	}

	float2 Material::GetDistortionScrollSpeed() const
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

	bool Material::HasTexture(const TextureType& e) const
	{
		return (mp_textures[e] != nullptr);
	}
	bool Material::HasTexture(UINT e) const
	{
		return (mp_textures[e] != nullptr);
	}
}

