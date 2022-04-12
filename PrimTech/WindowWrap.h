#pragma once
#include"DX11Wrapper.h"
#include <string>

class Window
{
public:
	Window();
	~Window();
	bool init(LPCWSTR windowName, HINSTANCE hInstance, std::wstring windowClass, unsigned int width = 1200, unsigned int height = 1000);
	bool processMsg();
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	std::wstring getWinName() const;
	HWND& getHWND();

	bool CreateDX11();
	
	uint16_t getWinWidth() const;
	uint16_t getWinHeight() const;
	void Run();
private:
	void Render();

	DX11Addon* m_pDX11 = nullptr;
	bool m_apiLoaded = false;
	int m_windowWidth, m_windowHeight = 0;
	std::wstring m_windowName;
	HWND m_hwnd = NULL;
	HINSTANCE m_hInstance = NULL;
	std::wstring m_wndClass;
};

