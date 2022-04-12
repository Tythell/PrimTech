#pragma once
#include <d3d11.h>
#pragma comment(lib, "D3DCompiler.lib")
#include <d3dcompiler.h>
#include "Utility\COMException.h"
class Shader
{
public:
	~Shader();
	ID3D10Blob* GetBuffer();
protected:
	bool ReadBlob(std::string path);
private:
	ID3D10Blob* mp_buffer;
	char* shaderByteCode;
};

class VShader : public Shader
{
public:
	~VShader();
	bool Init(ID3D11Device*& device, std::string shaderpath, D3D11_INPUT_ELEMENT_DESC* desc, UINT numElements);
	ID3D11VertexShader* GetShader();
	ID3D11InputLayout* GetInputLayout();
private:
	ID3D11VertexShader* m_vshader;
	ID3D11InputLayout* m_inputLayout;
};

class PShader : public Shader
{
public:
	~PShader();
	bool Init(ID3D11Device*& device, std::string shaderpath);
	ID3D11PixelShader* GetShader();
private:
	ID3D11PixelShader* m_pshader;
};

