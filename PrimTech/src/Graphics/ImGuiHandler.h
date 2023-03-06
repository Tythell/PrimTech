#pragma once
#include <Windows.h>
#include<d3d11.h>
#include "Buffer.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "ImGuizmo.h"

namespace PrimtTech
{
	class Material;

	struct SpotLight
	{
		float position[3]{ 0.f, 0.f, 0.f };
		float rotation[3]{ 0.f, -1.f, 0.f };
	};
	struct ImGuiVars
	{
		bool useVsync = true;
		float pointLightPos[3] = { 0.f,1.f,0.f };
		float pointLightColor[3]{ 1.f,1.f,1.f };
		float pointLightStr = 1.f;
		float pointLightDistance = 10.f;
		float offset = 0.f;
		float ambient[4] = { 1.f,1.f,1.f,.6f };
		float direction[3] = { 0.f, 1.f, 0.f };
		float atten = .5f;
		int fov = 80;
		float scale[3]{ 1.f,1.f,1.f };
		bool enableHandModel = false;
		bool showDemoWindow = false;
		bool showShadowMapDepth = false;
		bool showDebugWindow = true;
		float gradient[2] = { 255.f / 2.f,1.f };
		float gradientOffset = 0.f;
		float specPow = 50.f;
		bool drawRayCast = false;
		bool drawBCircle = true;
		ImGuizmo::MODE transformMode = ImGuizmo::MODE::LOCAL;
		bool showSelection = true;
		bool shadowMap = true;
		bool viewshadowcam = false;
		float shadowcamPos[3] = { 0.f,5.f,0.f };
		float shadowcamrotation[3] = { 0.f,0.f,0.f };
		SpotLight sl;
		float shadowBias = 0.005f;
		int points = 4;
		bool showKeybinds = false;
		uint width, height;
		bool m_isHoveringWindow = false;
		uint m_fps = 0;
	};

	struct GuiPtrs
	{
		CameraHandler* mp_camHandler = nullptr;
	};

	typedef void(*ImGuiWindowFunc)(void*);

	class ImGuiHandler
	{
	public:
		void ImGuiInit(HWND& hwnd, ID3D11Device*& device, ID3D11DeviceContext*& dc);
		void SetBufferPtrs(Buffer<hlsl::cbpLightBuffer>& lightBuffer, Buffer<hlsl::cbpMaterialBuffer>& materialBuffer)
		{
			m_pLightBuffer = &lightBuffer;
			m_pMatBuffer = &materialBuffer;
		}
		void SetPtrs(CameraHandler* cam) { m_pcamHandler = cam; };
		void LoadButton(Material* pMaterial, std::string name, unsigned int e, const unsigned int& i = 0);
		void ImGuiShutDown();
		void ImguiRender();
		void ImguiDebug();
		void CalculateFps(float deltatime);
		ImGuiVars* GetVarPtrs() { return &im; };

		void AddWindowFunc(ImGuiWindowFunc func, void* pVars);
	private:
		ImGuiVars im;
		Buffer<hlsl::cbpLightBuffer>* m_pLightBuffer;
		Buffer<hlsl::cbpMaterialBuffer>* m_pMatBuffer;

		CameraHandler* m_pcamHandler;
		std::vector<ImGuiWindowFunc> m_windows;
		std::vector<void*> m_funcVars;
	};
}



