#pragma once
#include <Windows.h>
#include<d3d11.h>
#include "Buffer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "ImGuizmo/ImGuizmo.h"

namespace PrimtTech
{
	typedef void(*ImGuiWindowFunc)(void*, bool*);

	class ImGuiHandler
	{
	public:
		void ImGuiInit(HWND& hwnd, ID3D11Device*& device, ID3D11DeviceContext*& dc);
		void ImGuiShutDown();
		void ImguiRender();

		void AddWindowFunc(ImGuiWindowFunc func, void* pVars, bool* showWin = (bool*)0);
	private:
		std::vector<ImGuiWindowFunc> m_windows;
		std::vector<void*> m_funcVars;
		std::vector<bool*> m_showWin;
	};
}



