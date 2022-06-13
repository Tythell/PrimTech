#pragma once
#include "Shader.h"
#include <Windows.h>
#include "../Utility/Popup.h"
#include"../Logic/CellGrid.h"
#include"Texture.h"
#include<wrl\client.h>
#include"../Input/Mouse.h"
#include"../Input/Keyboard.h"
#include"Model.h"

#include"3rdParty\imgui\imgui.h"
#include "3rdParty\imgui/imgui_impl_win32.h"
#include "3rdParty\imgui/imgui_impl_dx11.h"

namespace sm = DirectX::SimpleMath;
using Vector2i = DirectX::XMINT2;

class Window;

struct ImGuiVars
{
	float f[3] = {100.f,1.f, 0.f};
	bool pause = false;
	int speed = 70;
	char* buffer = new char[16]{"image.png"};
};

class DX11Addon
{
public:
	DX11Addon(Window& window, Camera& camera);
	~DX11Addon();


	void SetInputP(KeyboardHandler& kb);
	void Render();
	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;
private:
	bool initSwapChain();
	bool initRTV();
	bool SetupDSAndVP();
	bool InitRastNSampState();
	bool InitShaders();
	bool InitScene();
	void InitConstantBuffers();
	void UpdateConstantBuffers();
	void ImGuiInit(HWND& hwnd);
	void ImGuiRender();
	void ImGuiShutDown();
	void ExportImage(char* name);

	Window* m_pWin = nullptr;

	const unsigned int m_width, m_height;

	HWND* m_pHWND = nullptr;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* dc = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;

	ID3D11RenderTargetView* m_rtv = nullptr;
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_dsView = nullptr;
	ID3D11DepthStencilState* m_dsState;

	D3D11_VIEWPORT m_viewport;

	ID3D11RasterizerState* m_rasterizerState = nullptr;
	ID3D11SamplerState* m_sampState = nullptr;

	VertexShader m_quadVs;
	PixelShader m_quadPs;

	VertexShader m_3dvs;
	PixelShader m_3dps;

	//VertexBuffer<Vertex> m_vbuffer;
	//IndexBuffer m_iBuffer;

	//TextureMap m_fileTexture;

	Buffer<cbWorldTransforms3D> m_transformBuffer;

	Camera* mp_cam;

	//CellGrid m_grid;

	ImGuiVars im;
	KeyboardHandler* mp_kb;

	Model m_model;
	TextureMap m_missingTexture;
};

