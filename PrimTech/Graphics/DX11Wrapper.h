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
#include<fstream>

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
	char* buffer = new char[16]{"toon.png"};
	float pointLightPos[3] = { 0.f,1.f,0.f };
	float pointLightColor[3]{1.f,1.f,1.f};
	float pointLightStr = 1.f;
	float pointLightDistance = 10.f;
	float offset = 0.f;
	float ambient[4] = {1.f,1.f,1.f, .2f};
	float direction[3] = { 0.f, 1.f, 0.f };
	float atten = .5f;
	int fov = 80;
	float scale[3]{ 1.f,1.f,1.f };
	bool enableHandModel = false;
	bool showDemoWindow;
	float gradient[2] = { 255.f / 2.f,1.f};
	float gradientOffset = 0.f;
	float specPow = 50.f;
	bool showSelect = true;
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
	void ImGuiGradientWindow();
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
	//PixelShader m_3dps;
	PixelShader m_3dnoLightps;
	PixelShader m_toonPS;

	PixelShader m_linePS;
	VertexShader m_lineVS;

	//VertexBuffer<Vertex> m_vbuffer;
	//IndexBuffer m_iBuffer;

	//TextureMap m_fileTexture;

	Buffer<hlsl::cbpWorldTransforms3D> m_transformBuffer;
	Buffer<hlsl::cbpLightBuffer> m_lightbuffer;
	Buffer<hlsl::cbpMaterialBuffer> m_materialBuffer;

	//Mesh m_selectbox;

	Camera* mp_cam;

	//CellGrid m_grid;

	ImGuiVars im;
	KeyboardHandler* mp_kb;

	Model m_playermodel;
	Model m_bulb;
	Model m_model;
	Model m_plane;
	Model m_gunter;
	Model m_handmodel;
	Model m_water;
	Model m_cube;
	std::vector<Model> m_models;
	RenderBox m_renderbox;
	std::vector<std::string> m_modelNames;
	int m_fps = 0;

	unsigned char m_ZAToonExport[255] = {};
	const char** m_modelNamescStr;
};

