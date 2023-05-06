#include"Scene.h"

//#define _NEW new( _NORMAL_BLOCK, __FILE__, __LINE__)
//#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	//auto m = mono_init("dd");

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

	

	
	
	d::XMINT2 windowRes(1700, 900);

	Editor editor(windowRes, hInstance);

	editor.Run();

	return 0;
}