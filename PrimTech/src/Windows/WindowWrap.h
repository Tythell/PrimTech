#pragma once
#include "Graphics\DX11Renderer.h"
#include "Input/Mouse.h"
#include "Input/Keyboard.h"
#include "Utility/CommonDialogs.h"
#include <string>

namespace PrimtTech
{
	class Window
	{
	public:
		Window();
		~Window();
		bool init(LPCWSTR windowName, HINSTANCE hInstance, std::wstring windowClass, unsigned int width, unsigned int height);
		bool processMsg();
		LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		std::wstring getWinName() const;
		HWND& getHWND();

		uint2 GetWinDimensions() const;
		uint2* GetWinDimensionsP();
		void ShutDown();
		bool GetIsFocused() const;
		bool GetIsResize();
		void ToggleAlwaysOnTop(bool b);
	private:
		uint2 m_winDimension;
		uint2 m_windowPos = uint2(0);
		std::wstring m_windowName;
		HWND m_hwnd = NULL;
		HINSTANCE m_hInstance = NULL;
		std::wstring m_wndClass;

		bool m_isResize = false;
		bool m_isFocused = true;
		bool m_alwaysOnTop = false;
	};

}




