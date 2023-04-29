#include"Scene.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	//auto m = mono_init("dd");

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	d::XMINT2 windowRes(1700, 900);

	Editor editor(windowRes, hInstance);

#ifdef _DEBUG
	//HWND hwnd = GetConsoleWindow();
	//HMENU hmenu = GetSystemMenu(hwnd, false);
	//EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);

	//RECT rect;
	//GetWindowRect(hwnd, &rect);
	//int rWidth = rect.right - rect.left;
	//int rHeight = rect.bottom - rect.top;
	//SetWindowPos(hwnd, NULL, 0, 100, rWidth, rHeight, NULL);
#endif // _DEBUG

	editor.Run();

	return 0;
}