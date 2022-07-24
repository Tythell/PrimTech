#pragma once
#include"Graphics\DX11Wrapper.h"
#include "Input/Mouse.h"
#include"Input/Keyboard.h"
#include"Utility/CommonDialogs.h"
#include <string>

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
	void SetInputP(KeyboardHandler& pkb);

	bool CreateDX11();
	
	uint16_t getWinWidth() const;
	uint16_t getWinHeight() const;
	void ShutDown();
	bool GetIsFocused() const;
private:
	DX11Addon* m_pDX11 = nullptr;
	bool m_apiLoaded = false;
	int m_windowWidth = 0, m_windowHeight = 0;
	std::wstring m_windowName;
	HWND m_hwnd = NULL;
	HINSTANCE m_hInstance = NULL;
	std::wstring m_wndClass;
	KeyboardHandler* mp_kb = nullptr;
	bool m_isFocused = true;
};

