#include"Game.h"
#include <iostream>


#define _NEW new( _NORMAL_BLOCK, __FILE__, __LINE__)
//#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)


void EnableConsole(FILE* fp, bool enableConsole)
{
	if (enableConsole)
	{
		AllocConsole();
		freopen_s(&fp, "conin$", "r", stdin);
		freopen_s(&fp, "conout$", "w", stdout);
		freopen_s(&fp, "conout$", "w", stderr);
		printf("Console enabled:\n");
	}
}

void HandleCommandArgs(LPWSTR lpCmdLine, int& argc)
{
	LPWSTR* stringarr = CommandLineToArgvW(lpCmdLine, &argc);
	printf("command args:\n");
	for (int i = 0; i < argc; i++)
		wprintf(L"%i %s\n", i, stringarr[i]);

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	bool enableConsole = false;
	
	int argc = 0;
	FILE* fp = NULL;
	//HandleCommandArgs(lpCmdLine, argc);
	EnableConsole(fp, enableConsole);

	

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG


	int2 windowRes(1700, 900);

	Game game(windowRes, hInstance);
	game.Run();

	if (enableConsole)
	{
		fclose(fp);
		FreeConsole();
	}
	

	return 0;
}