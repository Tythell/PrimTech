#include "WindowWrap.h"
#include<omp.h>
#include<ctime>
#include<locale>
#include <iostream>

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
//int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	srand(time(0));

	int x = 200, y = 90;
	//std::cout << "input x: ";
	//std::cin >> x;

	//std::cout << "input y: ";
	//std::cin >> y;

	Window window;
	window.init(L"PrimTech", NULL, L"windowClass", 1400, 800);

	int numArgs = 0;
	//LPWSTR* commandArgs = CommandLineToArgvW(lpCmdLine, &numArgs);

	//std::wstring s = commandArgs[1];



	//int x = wcstol(s.c_str(), );

	//DX11Addon dx(window);
	//window.BindAPI(dx);
	window.CreateDX11(x, y);

	window.Run();

	return 0;
}
