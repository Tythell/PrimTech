#pragma once
#include <d3d11.h>
#pragma comment(lib, "D3DCompiler.lib")
#include <d3dcompiler.h>
#include "../Utility/Popup.h"
class Shader
{
public:
	~Shader();
	ID3D10Blob* GetBuffer();
	virtual bool Init(ID3D11Device*& device, std::string shaderpath) = 0;
protected:
	bool ReadBlob(std::string path);
private:
	ID3D10Blob* mp_buffer = nullptr;
};

class VertexShader : public Shader
{
public:
	~VertexShader();
	bool InitInputLayout(ID3D11Device*& device, D3D11_INPUT_ELEMENT_DESC* desc, UINT numElements);
	bool Init(ID3D11Device*& device, std::string shaderpath) override;
	ID3D11VertexShader* GetShader();
	ID3D11InputLayout* GetInputLayout();
private:
	ID3D11VertexShader* m_vshader = nullptr;
	ID3D11InputLayout* m_inputLayout = nullptr;
};

class PixelShader : public Shader
{
public:
	~PixelShader();
	bool Init(ID3D11Device*& device, std::string shaderpath) override;
	ID3D11PixelShader* GetShader();
private:
	ID3D11PixelShader* m_pshader = nullptr;
};

