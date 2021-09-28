#include "WindowWrap.h"

int WINAPI wWinMain(/*_In_*/ HINSTANCE hInstance, /*_In_opt_*/ HINSTANCE hPrevInstance, /*_In_*/ LPWSTR lpCmdLine, /*_In_*/ int nCmdShow)
{
	Window window;

	window.init(L"PrimTech", hInstance, L"windowClass");

	while (window.processMsg())
	{

	}

	return 0;
}
