#include "PrimTech.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
//#ifdef _DEBUG
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif // _DEBUG

	pt::PrimTech app;
	app.Init(L"PrimTech Editor", hInstance, L"windowClass", 1800, 950);
	app.Run();

	return 0;
}
