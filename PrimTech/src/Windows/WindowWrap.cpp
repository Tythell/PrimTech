#include"pch.h"
#include "WindowWrap.h"
#include <memory>
#include<omp.h>

LRESULT CALLBACK MessageDirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
#ifdef _DEBUG
		if (MessageBox(hwnd, L"Leaving already?", L"[REDACTED]", MB_OKCANCEL) == IDOK)
		{
			MessageBox(hwnd, L"lmao you wish", L"[REDACTED]", 0);
			//DestroyWindow(hwnd);
		}
#else
		DestroyWindow(hwnd);
#endif // _DEBUG
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
	{
		Window* const pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* pWindow = reinterpret_cast<Window*>(pCreate->lpCreateParams);
		if (pWindow == nullptr)
		{
			Popup::Error("Critical error: Pointer to window is null");
			exit(-1);
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

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) return true;
	switch (uMsg)
	{
	case WM_ACTIVATE:
	{
		m_isFocused = (LOWORD(wParam));
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		MouseHandler::SetMouseButton(eLEFTCLICK, true);
		MouseEvent e(MouseEvent::EventType::eLEFTCLICK, x, y);
		MouseHandler::AddMouseEvent(e);
		return 0;
	}
	//case WM_LBUTTONDBLCLK: // enable in window style
	//{
	//	int x = LOWORD(lParam);
	//	int y = HIWORD(lParam);
	//	return 0;
	//}
	case WM_RBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		MouseHandler::SetMouseButton(eRIGHTCLICK, true);
		MouseHandler::AddMouseEvent(MouseEvent(MouseEvent::EventType::eRIGHTCLICK, x, y));
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		MouseHandler::SetMouseButton(eMIDCLICK, true);
		MouseHandler::AddMouseEvent(MouseEvent(MouseEvent::EventType::eMIDCLICK, x, y));
		return 0;
	}
	case WM_LBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		//m_mouseHandler.SetMouseButton(eLEFTCLICK, false);
		MouseHandler::SetMouseButton(eLEFTCLICK, false);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		//m_mouseHandler.SetMouseButton(eLEFTCLICK, false);
		MouseHandler::SetMouseButton(eRIGHTCLICK, false);

		return 0;
	}
	case WM_MBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		//m_mouseHandler.SetMouseButton(eLEFTCLICK, false);
		MouseHandler::SetMouseButton(eMIDCLICK, false);
		return 0;
	}
	case WM_KEYDOWN:
	{
		unsigned char key = static_cast<unsigned char>(wParam);
		//KeyboardHandler::SetKeyState(key, true);
		mp_kb->AddKeyboardEvent(KeyboardEvent(KeyboardEvent::EventType::ePUSH, key));
		mp_kb->SetKeyState(key, true);
		return 0;
	}
	case WM_KEYUP:
	{
		unsigned char key = static_cast<unsigned char>(wParam);
		//KeyboardHandler::SetKeyState(key, false);
		mp_kb->AddKeyboardEvent(KeyboardEvent(KeyboardEvent::EventType::eRELEASE, key));
		mp_kb->SetKeyState(key, false);
		return 0;

	}
	case WM_CHAR:
	{
		unsigned char key = static_cast<unsigned char>(wParam);
		if (mp_kb->IsRecording())
		{
			mp_kb->AddKeyboardEvent(KeyboardEvent(KeyboardEvent::EventType::eCHAR, key));
		}
		return 0;

	}
	case WM_INPUT:
	{
		UINT dataSize = 0;
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));
		if (dataSize > 0)
		{
			std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
			{
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
				if (raw->header.dwType == RIM_TYPEMOUSE)
					MouseHandler::OnMouseRaw((float)raw->data.mouse.lLastX, (float)raw->data.mouse.lLastY);
			}
		}
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) MouseHandler::OnWheelUp(x, y);
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) MouseHandler::OnWheelDown(x, y);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

std::wstring Window::getWinName() const
{
	return m_windowName;
}

HWND& Window::getHWND()
{
	return m_hwnd;
}

void Window::SetInputP(KeyboardHandler& pkb)
{
	mp_kb = &pkb;
}

uint16_t Window::getWinWidth() const
{
	return m_windowWidth;
}

uint16_t Window::getWinHeight() const
{
	return m_windowHeight;
}

void Window::ShutDown()
{
	DestroyWindow(m_hwnd);
}

bool Window::GetIsFocused() const
{
	return m_isFocused;
}

Window::Window() :
	m_windowWidth(0), m_windowHeight(0)
{
	static bool rawInputInitialized = false;
	if (!rawInputInitialized)
	{
		RAWINPUTDEVICE rid;

		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = 0;
		rid.hwndTarget = NULL;
		if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
			COM_ERROR(GetLastError(), "Failed to register raw mouse");
		rawInputInitialized = true;
	}
}

Window::~Window()
{
}

bool Window::init(LPCWSTR windowName, HINSTANCE hInstance, std::wstring windowClass, unsigned int width, unsigned int height)
{
	m_hInstance = hInstance;
	m_windowName = windowName;
	// Register window class
	m_wndClass = windowClass;

	m_windowWidth = width;
	m_windowHeight = height;

	WNDCLASS wc = {};

	//wc.style = CS_DBLCLKS | CS_NOCLOSE; // om vi vill att fönstret ska registrera dubbel klicks
	wc.lpfnWndProc = HandleMessageSetup;
	wc.hInstance = hInstance;
	wc.lpszClassName = m_wndClass.c_str();

	RECT rect = {};
	rect.left = LONG(GetSystemMetrics(SM_CXSCREEN) * .5f - width * .5f);
	rect.top = LONG(GetSystemMetrics(SM_CYSCREEN) * .5f - height * .5f);
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	DWORD style = WS_OVERLAPPEDWINDOW;

	AdjustWindowRect(&rect, style, false);

	//m_windowWidth = rect.right - rect.left;
	//m_windowHeight = rect.bottom - rect.top;

	RegisterClass(&wc);
	m_hwnd = CreateWindowEx(0, m_wndClass.c_str(), m_windowName.c_str(), style,
		(LONG(GetSystemMetrics(SM_CXSCREEN) - m_windowWidth) / 2),
		(LONG(GetSystemMetrics(SM_CYSCREEN) - m_windowHeight) / 3),
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL, NULL, hInstance, this);

	if (m_hwnd == NULL) return false;

	Dialogs::SetOwnerHandle(&m_hwnd);

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
	return true;
}