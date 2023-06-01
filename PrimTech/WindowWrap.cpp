#include "WindowWrap.h"
#include<omp.h>

LRESULT CALLBACK MessageDirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	//if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) return true;

	Window* const pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	switch (uMsg)
	{
	case WM_CLOSE:
		//if (MessageBox(hwnd, L"Leaving already?", L"[REDACTED]", MB_OKCANCEL) == IDOK)
		//{
		//	MessageBox(hwnd, L"NO!", L"You can't leave", 0);
		//	//DestroyWindow(hwnd);
		//}
		DestroyWindow(hwnd);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
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
			UnregisterClass(m_wndClass.c_str(), m_hInstance);
			return false;
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

bool Window::BindAPI(DX11Addon& api)
{
	if (m_apiLoaded) return false;
	m_pDX11 = &api;
	return true;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) return true;
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	{
		MouseHandler::SetMouseButton(eLEFTCLICK, true);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		MouseHandler::SetMouseButton(eRIGHTCLICK, true);
		break;
	}
	case WM_MBUTTONDOWN:
	{
		MouseHandler::SetMouseButton(eMIDCLICK, true);
		break;
	}
	case WM_LBUTTONUP:
	{
		//m_mouseHandler.SetMouseButton(eLEFTCLICK, false);
		MouseHandler::SetMouseButton(eLEFTCLICK, false);
		break;
	}
	case WM_RBUTTONUP:
	{
		//m_mouseHandler.SetMouseButton(eLEFTCLICK, false);
		MouseHandler::SetMouseButton(eRIGHTCLICK, false);
		break;
	}
	case WM_MBUTTONUP:
	{
		//m_mouseHandler.SetMouseButton(eLEFTCLICK, false);
		MouseHandler::SetMouseButton(eMIDCLICK, false);
		break;;
	}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

std::wstring Window::getWinName() const
{
	return m_windowName;
}

HWND& Window::getHWND()
{
	return m_hwnd;
}

bool Window::CreateDX11(int x, int y)
{
	if (m_apiLoaded) return false;
	m_pDX11 = new DX11Addon(*this, x, y);
	m_apiLoaded = true;
	return true;
}

uint16_t Window::getWinWidth() const
{
	return m_windowWidth;
}

uint16_t Window::getWinHeight() const
{
	return m_windowHeight;
}

void Window::Run()
{
	double start, deltatime = 0.f;
	while (processMsg())
	{
		start = omp_get_wtime();
		m_pDX11->Render(deltatime);
		deltatime = omp_get_wtime() - start;
	}
}

Window::Window():
	m_windowWidth(0), m_windowHeight(0)
{

}

Window::~Window()
{
	if (m_pDX11) delete m_pDX11;
}

bool Window::init(LPCWSTR windowName, HINSTANCE hInstance, std::wstring windowClass, unsigned int width, unsigned int height)
{

	m_windowName = windowName;
	// Register window class
	m_wndClass = windowClass;

	m_windowWidth = width;
	m_windowHeight = height;

	WNDCLASS wc = {};

	//wc.style = CS_DBLCLKS; // om vi vill att fönstret ska registrera dubbel klicks
	wc.lpfnWndProc = HandleMessageSetup;
	wc.hInstance = hInstance;
	wc.lpszClassName = m_wndClass.c_str();

	RECT rect = {};
	rect.left = GetSystemMetrics(SM_CXSCREEN) * .5f - width * .5f;
	rect.top = GetSystemMetrics(SM_CYSCREEN) * .5f - height * .5f;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	DWORD style = WS_OVERLAPPEDWINDOW;

	AdjustWindowRect(&rect, style, false);

	//m_windowWidth = rect.right - rect.left;
	//m_windowHeight = rect.bottom - rect.top;

	RegisterClass(&wc);
	m_hwnd = CreateWindowEx(0, m_wndClass.c_str(), m_windowName.c_str(), style,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL, NULL, hInstance, NULL);
	
	



	if (m_hwnd == NULL) return false;

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
	return true;
}
