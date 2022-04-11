#include "WindowWrap.h"

int WINAPI wWinMain(/*_In_*/ HINSTANCE hInstance, /*_In_opt_*/ HINSTANCE hPrevInstance, /*_In_*/ LPWSTR lpCmdLine, /*_In_*/ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Window window;

	window.init(L"PrimTech", hInstance, L"windowClass", 1000, 800);
	window.CreateDX11();

	while (window.processMsg())
	{
		
	}

	return 0;
}
