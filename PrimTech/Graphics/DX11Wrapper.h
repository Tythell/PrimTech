#pragma once
#include "Shader.h"
#include <Windows.h>
#include "../Utility/Popup.h"
#include"RenderCell.h"
#include"Texture.h"
#include<wrl\client.h>

#include"3rdParty\imgui\imgui.h"
#include "3rdParty\imgui/imgui_impl_win32.h"
#include "3rdParty\imgui/imgui_impl_dx11.h"

namespace sm = DirectX::SimpleMath;
using Vector2i = DirectX::XMINT2;

class Window;

struct ImGuiVars
{
	float f[3] = {50.f,1.f, 0.f};
};

class DX11Addon
{
public:
	DX11Addon(Window& window);
	~DX11Addon();


	void Render();
private:
	bool initSwapChain();
	bool initRTV();
	bool SetupDSAndVP();
	bool InitRastNSampState();
	bool InitShaders();
	bool InitScene();
	void InitConstantBuffers();
	void UpdateConstantBuffers();

	Window* m_pWin = nullptr;

	const unsigned int m_width, m_height;

	HWND* m_pHWND = nullptr;

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_dc = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;

	ID3D11RenderTargetView* m_rtv = nullptr;
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_dsView = nullptr;
	ID3D11DepthStencilState* m_dsState;

	D3D11_VIEWPORT m_viewport;

	ID3D11RasterizerState* m_rasterizerState = nullptr;
	ID3D11SamplerState* m_sampState = nullptr;

	VShader m_vShader;
	PShader m_pShader;

	VertexBuffer<Vertex> m_vbuffer;
	IndexBuffer m_iBuffer;

	//TextureMap m_fileTexture;

	ConstantBuffer<Transforms> m_transformBuffer;
	Camera m_cam;

	RenderCell m_pixel;

	ImGuiVars Im;
};

