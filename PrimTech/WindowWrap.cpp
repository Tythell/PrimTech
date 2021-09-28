#include "WindowWrap.h"

LRESULT CALLBACK MessageDirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;
	default:
		Window* const pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Window* pWin;
	//if (uMsg == WM_NCCREATE)
	//{
	//	pWin = (Window*)reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams;
	//	SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWin));
	//}
	//else
	//{
	//	pWin = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	//}
	//if (pWin) 
	//	return pWin->WindowProc(hwnd, uMsg, wParam, lParam);
	//else
	//	return DefWindowProc(hwnd, uMsg, wParam, lParam);
	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* pWindow = reinterpret_cast<Window*>(pCreate->lpCreateParams);
		if (pWindow)
		{
			throw;
		}
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(MessageDirect));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}


Window::Window():
	m_width(0), m_height(0)
{}

bool Window::processMsg()
{
	MSG msg = {};
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL)
	{
		if (!IsWindow(this->m_hwnd))
		{
			m_hwnd = NULL;
			UnregisterClass(m_wndClass.c_str(), );
		}
	}

	//MSG msg = {};
	//while (GetMessage(&msg, NULL, 0, 0))
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}

	return true;
}

LRESULT Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		if (MessageBox(hwnd, L"Leaving already?", m_windowName.c_str(), MB_OKCANCEL) == IDOK)
		{
			MessageBoxW(NULL, L"NO!", m_windowName.c_str(), MB_ICONERROR);
		}
		return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

std::wstring Window::getWinName() const
{
	return m_windowName;
}

bool Window::init(std::wstring windowName, HINSTANCE hInstance, std::wstring windowClass)
{
	m_windowName = windowName;
	// Register window class
	m_wndClass = windowClass;

	WNDCLASS wc = {};

	wc.lpfnWndProc = HandleMessageSetup;
	wc.hInstance = hInstance;
	wc.lpszClassName = wndClass.c_str();

	RegisterClass(&wc);

	// skapa window
	m_hwnd = CreateWindowEx(0, wndClass.c_str(), m_windowName.c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	if (m_hwnd == NULL) return 0;

	ShowWindow(m_hwnd, SW_SHOW);

}
