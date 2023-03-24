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
	struct ImGuiVars
	{
		bool showDebugWindow = true;
		bool useVsync = false;
		float pointLightDistance = 10.f;
		float direction[3] = { 0.f, 1.f, 0.f };
		float shadowBias = 0.005f;
		uint width, height;
		bool m_isHoveringWindow = false;
		bool m_drawGrid = true;
	};

	typedef void(*ImGuiWindowFunc)(void*, bool*);

	class ImGuiHandler
	{
	public:
		void ImGuiInit(HWND& hwnd, ID3D11Device*& device, ID3D11DeviceContext*& dc);
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



