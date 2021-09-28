#pragma once
#include <Windows.h>
#include <string>
class Window
{
private:
	std::wstring m_windowName;
	const int m_width, m_height;
	HWND m_hwnd;
	HINSTANCE m_hInstance = NULL;
	std::wstring m_wndClass;
public:
	Window();
	bool init(std::wstring windowName, HINSTANCE hInstance, std::wstring windowClass);
	bool processMsg();
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	std::wstring getWinName() const;
};

