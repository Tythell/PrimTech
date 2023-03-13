#pragma once
#include "pch.h"

#include "Shader.h"
#include"ShadowMap.h"
#include "../Utility/Popup.h"
#include"Texture.h"
#include"../Input/Mouse.h"
#include"../Input/Keyboard.h"
#include"ViewModel.h"
#include "../ecs/Entity.h"

#pragma warning(push)
#pragma warning(disable : 26812)

#include "ImGuiHandler.h"

#define IMGUI_DEFINE_MATH_OPERATORS

namespace sm = DirectX::SimpleMath;

namespace PrimtTech
{
	using Vector2i = DirectX::XMINT2;

	class Window;

	class DX11Renderer
	{
	public:
		DX11Renderer(Window& window, CameraHandler& camera);
		~DX11Renderer();
		void SetImGuiHandler(ImGuiHandler& gui);

		void SetInputP(KeyboardHandler& kb);
		void Render(const float& deltatime);
		ID3D11Device* GetDevice() const;
		ID3D11DeviceContext* GetDeviceContext() const;
		void ShutDown();
		void UpdateScene(const float& deltatime);

		ImGuiHandler* GetGuiHandlerP() { return m_guiHandler; }

		void SetLightWarp(const std::string& path);
	private:
		bool initSwapChain();
		bool initRTV();
		bool SetupDSAndVP();
		bool InitRastNSampState();
		void InitBlendState();
		bool InitShaders();
		bool InitScene();
		void InitConstantBuffers();

		void ImGuiRender();
		void ImGuTextureDisplay();

		//void ImportScene(std::string path);
		void ExportScene(std::string path);

		Window* m_pWin = nullptr;

		const unsigned int m_width, m_height;

		HWND* m_pHWND = nullptr;

		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* dc = nullptr;
		IDXGISwapChain* m_swapChain = nullptr;

		ID3D11RenderTargetView* m_rtv = nullptr;
		ID3D11Texture2D* m_depthStencilBuffer = nullptr;
		ID3D11DepthStencilView* m_dsView = nullptr;
		ID3D11DepthStencilState* m_dsState = nullptr;

		ID3D11RasterizerState* m_rasterizerState = nullptr;
		ID3D11RasterizerState* m_wireFrameState = nullptr;
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

		Buffer<BBVertex> m_grid;

		CameraHandler* mp_camHandler = nullptr;
		Camera* mp_currentCam = nullptr;
		uint m_activeCamIndex = 0;

		ImGuiHandler* m_guiHandler = nullptr;
		ImGuiVars* im = nullptr;
		GuiPtrs m_ptrs;

		KeyboardHandler* mp_kb = nullptr;

		int m_selected = -1;
		int m_selectedMtrl = -1;
		RenderBox m_renderbox;
		sm::Ray m_ray;
		int m_fps = 0;
		RenderLine m_rLine;
		RenderSphere m_sphere;

		ShadowMap m_shadowmap;
	};
}