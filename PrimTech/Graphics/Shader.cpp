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
	path = SHADER_DIR + path;
	HRESULT hr = D3DReadFileToBlob(StringHelper::StringToW(path).c_str(), &mp_buffer);
	std::string error = "Failed to load shader: " + path;
	COM_ERROR(hr, error);
	return false;
}

VertexShader::~VertexShader()
{
	m_vshader->Release();
	m_inputLayout->Release();
}

bool VertexShader::InitInputLayout(ID3D11Device*& device, D3D11_INPUT_ELEMENT_DESC* desc, UINT numElements)
{
	HRESULT hr = device->CreateInputLayout(desc, numElements, GetBuffer()->GetBufferPointer(), GetBuffer()->GetBufferSize(), &m_inputLayout);
	COM_ERROR(hr, "Failed to create input layout");
	return true;
}

bool VertexShader::Init(ID3D11Device*& device, std::string shaderpath)
{
	ReadBlob(shaderpath);
	HRESULT hr = device->CreateVertexShader(GetBuffer()->GetBufferPointer(), GetBuffer()->GetBufferSize(), NULL, &m_vshader);
	std::string error = "Failed to create shader: " + shaderpath;
	COM_ERROR(hr, error.c_str());

	return true;
}

ID3D11VertexShader* VertexShader::GetShader()
{
	return m_vshader;
}

ID3D11InputLayout* VertexShader::GetInputLayout()
{
	return m_inputLayout;
}

PixelShader::~PixelShader()
{
	m_pshader->Release();
}

bool PixelShader::Init(ID3D11Device*& device, std::string shaderpath)
{
	ReadBlob(shaderpath);
	HRESULT hr = device->CreatePixelShader(GetBuffer()->GetBufferPointer(), GetBuffer()->GetBufferSize(), NULL, &m_pshader);
	std::string error = "Failed to create shader: " + shaderpath;
	COM_ERROR(hr, error.c_str());
	return true;
}

ID3D11PixelShader* PixelShader::GetShader()
{
	return m_pshader;
}