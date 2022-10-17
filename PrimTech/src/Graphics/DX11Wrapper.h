#pragma once
#include<d3d11.h>
#include "Shader.h"
#include"ShadowMap.h"
#include <Windows.h>
#include "../Utility/Popup.h"
#include"Texture.h"
#include"../Input/Mouse.h"
#include"../Input/Keyboard.h"
#include"ViewModel.h"
#include<fstream>
#pragma warning(push)
#pragma warning(disable : 26812)
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

namespace sm = DirectX::SimpleMath;
using Vector2i = DirectX::XMINT2;
using ModelList = std::vector<Model*>;

class Window;

struct SpotLight
{
	float position[3]{ 0.f, 0.f, 0.f };
	float rotation[3] {0.f, -1.f, 0.f};
};

struct ImGuiVars
{
	bool mayaOn = true;
	bool useVsync = true;
	char* buffer = new char[16]{"toon.png"};
	float pointLightPos[3] = { 0.f,1.f,0.f };
	float pointLightColor[3]{1.f,1.f,1.f};
	float pointLightStr = 1.f;
	float pointLightDistance = 10.f;
	float offset = 0.f;
	float ambient[4] = {1.f,1.f,1.f,.6f};
	float direction[3] = { 0.f, 1.f, 0.f };
	float atten = .5f;
	int fov = 80;
	float scale[3]{ 1.f,1.f,1.f };
	bool enableHandModel = false;
	bool showDemoWindow = false;
	bool showShadowMapDepth = false;
	bool showDebugWindow = true;
	float gradient[2] = { 255.f / 2.f,1.f};
	float gradientOffset = 0.f;
	float specPow = 50.f;
	bool drawRayCast = false;
	bool drawBCircle = false;
	bool showSelection = true;
	bool shadowMap = true;
	bool viewshadowcam = false;
	float shadowcamPos[3] = {0.f,5.f,0.f};
	float shadowcamrotation[3] = { d::XM_PIDIV2,0.f,0.f};
	SpotLight sl;
	float shadowBias = 0.005f;
};

void RecursiveRead(Sceneheaders& header, ModelList& v, std::ifstream& reader);

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

	void Click(const sm::Vector3& dir);
	void SetCanMove(bool b);

	void AddNewModel(const std::string name, std::vector<Vertex3D>& vertexArray, std::vector<Model*>& v);
	void MoveVertex(const std::string name, const uint& id, std::vector<Model*> v);
	ModelList GetModelList();
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
	void ImguiDebug();
	void ImGuiMenu();
	void ImGuiEntList();
	void ImGuTextureDisplay();

	void ImportScene(std::string path);
	void ExportScene(std::string path);
	void NewScene();
	void ClearModelList();
	void SetLightWarp(const std::string& path);

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
	ID3D11SamplerState* m_wrapSampler = nullptr;
	ID3D11SamplerState* m_clampSampler = nullptr;
	ID3D11SamplerState* m_shadowSampler = nullptr;
	ID3D11BlendState* m_blendState = nullptr;

	CD3D11_VIEWPORT m_viewport;

	VertexShader m_3dvs;
	PixelShader m_3dnoLightps;
	PixelShader m_toonPS;

	PixelShader m_linePS;
	VertexShader m_lineVS;

	Buffer<hlsl::cbpWorldTransforms3D> m_transformBuffer;
	Buffer<hlsl::cbpLightBuffer> m_lightbuffer;
	Buffer<hlsl::cbpMaterialBuffer> m_materialBuffer;

	Camera* mp_cam;

	ImGuiVars im;
	KeyboardHandler* mp_kb;

	int m_selected = -1;
	Model m_bulb;
	Model m_spotlight;
	ViewModel m_viewmdl;
	Model m_playermodel;
	ModelList m_models;
	RenderBox m_renderbox;
	sm::Ray m_ray;
	int m_fps = 0;
	RenderLine m_rLine;
	RenderSphere m_sphere;

	sm::Vector2 mouseClickPos = {0.f,0.f};

	//unsigned char m_ZAToonExport[255] = {};
	bool m_isHoveringWindow = false;
	bool m_canMove = true;
	ShadowMap m_shadowmap;
};

