#include "Shader.h"

Shader::~Shader()
{
	mp_buffer->Release();
}

ID3D10Blob* Shader::GetBuffer()
{
	return mp_buffer;
}

bool Shader::ReadBlob(std::string path)
{
	std::wstring lstring = StringHelper::StringToW(path);
	HRESULT hr = D3DReadFileToBlob(lstring.c_str(), &mp_buffer);
	std::string error = "Failed to load shader: " + path;
	COM_ERROR(hr, error);
	return false;
}

VShader::~VShader()
{
	m_vshader->Release();
	m_inputLayout->Release();
}

bool VShader::Init(ID3D11Device*& device, std::string shaderpath, D3D11_INPUT_ELEMENT_DESC* desc, UINT numElements)
{
	ReadBlob(shaderpath);
	HRESULT hr = device->CreateVertexShader(GetBuffer()->GetBufferPointer(), GetBuffer()->GetBufferSize(), NULL, &m_vshader);
	std::string error = "Failed to create shader: " + shaderpath;
	COM_ERROR(hr, error.c_str());

	hr = device->CreateInputLayout(desc, numElements, GetBuffer()->GetBufferPointer(), GetBuffer()->GetBufferSize(), &m_inputLayout);
	COM_ERROR(hr, "Failed to create input layout");
	return true;
}

ID3D11VertexShader* VShader::GetShader()
{
	return m_vshader;
}

ID3D11InputLayout* VShader::GetInputLayout()
{
	return m_inputLayout;
}

PShader::~PShader()
{
	m_pshader->Release();
}

bool PShader::Init(ID3D11Device*& device, std::string shaderpath)
{
	ReadBlob(shaderpath);
	HRESULT hr = device->CreatePixelShader(GetBuffer()->GetBufferPointer(), GetBuffer()->GetBufferSize(), NULL, &m_pshader);
	std::string error = "Failed to create shader: " + shaderpath;
	COM_ERROR(hr, error.c_str());
	return true;
}

ID3D11PixelShader* PShader::GetShader()
{
	return m_pshader;
}
