#pragma once
#include "pch.h"

#include "Shader.h"
#include"ShadowMap.h"
#include "../Utility/Popup.h"
#include"AssetLoading/Texture.h"
#include"../Input/Mouse.h"
#include"../Input/Keyboard.h"
#include "Physics/PhysicsHandler.h"
#include "ecs/Entity.h"
#include "RenderShapes.h"

#pragma warning(push)
#pragma warning(disable : 26812)

#include "ImGuiHandler.h"

#define IMGUI_DEFINE_MATH_OPERATORS

//namespace sm = DirectX::SimpleMath;

namespace PrimtTech
{
	class Window;

	class Renderer
	{
	public:
		Renderer(Window& window);
		~Renderer();

		void Render(const float& deltatime);
		ID3D11Device* GetDevice() const;
		ID3D11DeviceContext* GetDeviceContext() const;
		void ShutDown();

		ImGuiHandler* GetGuiHandlerP() { return &m_guiHandler; }
		ImGuiHandler& GetGuiHandlerR() { return m_guiHandler; }

		void SetLightWarp(const std::string& path);
		void SetActiveCam(uint idx);

		void UpdateWindowCall(pt::Camera& cam);
	private:
		bool initSwapChain();
		bool initRTV();
		bool SetupDS();
		bool InitRastNSampState();
		void InitBlendState();
		bool InitShaders();
		bool InitScene();
		void InitConstantBuffers();

		void ImGuiRender();
		//void ImGuTextureDisplay();

#ifdef PHYSDEBUG
		PhysicsHandler* mp_debufrenderer = nullptr;

	public:
		void SetDebugRenderer(PhysicsHandler* p) { mp_debufrenderer = p; }
	private:
		bool m_updateWinHeight = false;
		uint m_drawCalls = 0;

#endif // PHYSDEBUG
		Window* m_pWin = nullptr;

		uint2* m_winDimensions;

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
		//PixelShader m_3dnoLightps;
		PixelShader m_toonPS;

		PixelShader m_linePS;
		VertexShader m_lineVS;

		Buffer<hlsl::cbpWorldTransforms3D> m_transformBuffer;
		Buffer<hlsl::cbpLightBuffer> m_lightbuffer;
		Buffer<hlsl::cbpMaterialBuffer> m_materialBuffer;

		std::vector<hlsl::Light> m_lightVector;
		Buffer<hlsl::Light> m_multiLightBuffer;
		//Buffer<BBVertex> m_grid;

		uint m_activeCamIndex = 1;

		ImGuiHandler m_guiHandler;
		//ImGuiVars* im = nullptr;

		//RenderBox m_renderbox;

		int m_fps = 0;

		ShadowMap m_shadowmap;
		uint m_shadowCamIndex = 0;
	};
}