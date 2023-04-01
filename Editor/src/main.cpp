#include"Scene.h"

#include <mono/jit/jit.h>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	//auto m = mono_init("dd");

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	d::XMINT2 windowRes(1700, 900);

	Editor editor(windowRes, hInstance);
	editor.Run();

	return 0;
}