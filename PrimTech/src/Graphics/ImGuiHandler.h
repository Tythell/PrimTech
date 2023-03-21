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
		float pointLightDistance = 10.f;
		float ambient[4] = { 1.f,1.f,1.f,.6f };
		float direction[3] = { 0.f, 1.f, 0.f };
		float shadowBias = 0.005f;
		uint width, height;
		bool m_isHoveringWindow = false;
	};

	typedef void(*ImGuiWindowFunc)(void*, bool*);

	class ImGuiHandler
	{
	public:
		void ImGuiInit(HWND& hwnd, ID3D11Device*& device, ID3D11DeviceContext*& dc);
		void LoadButton(Material* pMaterial, std::string name, unsigned int e, const unsigned int& i = 0);
		void ImGuiShutDown();
		void ImguiRender();
		ImGuiVars* GetVarPtrs() { return &im; };

		void AddWindowFunc(ImGuiWindowFunc func, void* pVars, bool* showWin = (bool*)0);
	private:
		ImGuiVars im;

		std::vector<ImGuiWindowFunc> m_windows;
		std::vector<void*> m_funcVars;
		std::vector<bool*> m_showWin;
	};
}



