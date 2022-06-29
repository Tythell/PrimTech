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
	bool useVsync = true;
	int speed = 70;
	char* buffer = new char[16]{"image.png"};
	float pointLightPos[3] = { 0.f,1.f,0.f };
	float pointLightColor[3]{1.f,1.f,1.f};
	float pointLightStr = 1.f;
	float offset = 0.f;
	float ambient[4] = {1.f,1.f,1.f, .2f};
	float direction[3] = { 1.f,1.f,1.f };
	float atten = 1.f;
	int fov = 80.f;
	float scale[3]{ 1.f,1.f,1.f };
	bool enableHandModel = false;
	float diffuseScrollSpeed[2] = { -0.285f, 0.5f };
	float distScrollSpeed[2] = { 0.5f, -0.5f };
	int distDiv = 5;
};

class DX11Addon
{
public:
	DX11Addon(Window& window, Camera& camera);
	~DX11Addon();

	void SetInputP(KeyboardHandler& kb);
	void Render(const float& deltatime);
	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;
	void ShutDown();
	void UpdateScene(const float& deltatime);
	void CalculateFps(const float& deltatime);
private:
	bool initSwapChain();
	bool initRTV();
	bool SetupDSAndVP();
	bool InitRastNSampState();
	void InitBlendState();
	bool InitShaders();
	bool InitScene();
	void InitConstantBuffers();
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

	ID3D11RasterizerState* m_rasterizerState = nullptr;
	ID3D11SamplerState* m_sampState = nullptr;
	ID3D11BlendState* m_blendState = nullptr;

	//VertexShader m_quadVs;
	//PixelShader m_quadPs;

	VertexShader m_3dvs;
	PixelShader m_3dps;
	PixelShader m_3dnoLightps;

	//VertexBuffer<Vertex> m_vbuffer;
	//IndexBuffer m_iBuffer;

	//TextureMap m_fileTexture;

	Buffer<hlsl::cbpWorldTransforms3D> m_transformBuffer;
	Buffer<hlsl::cbpLightBuffer> m_lightbuffer;
	Buffer<hlsl::cbpMaterialBuffer> m_materialBuffer;

	Camera* mp_cam;

	//CellGrid m_grid;

	ImGuiVars im;
	KeyboardHandler* mp_kb;

	Model m_model;
	Model m_plane;
	Model m_bulb;
	Model m_playermodel;
	Model m_gunter;
	Model m_menacing;
	Model m_handmodel;
	Model m_water;
	int m_fps = 0;
};

