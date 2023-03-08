#include"Scene.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	pt::PrimTech pt;

	//PrimtTech::ResourceHandler::ReserveMeshMemory(10);
	PrimtTech::ResourceHandler::ReserveMaterialMemory(8);

	d::XMINT2 windowRes(1700, 1000);
	pt.Init(L"Editor", hInstance, L"wndc", windowRes.x, windowRes.y);

	Editor scene(pt.GetRenderer()->GetGuiHandlerP(), windowRes);

	double start = 0, deltaTime = 0;
	start = omp_get_wtime();

	while (pt.IsOpen())
	{
		start = omp_get_wtime();
		pt.Run();
		scene.Update((float)deltaTime);

		deltaTime = omp_get_wtime() - start;
		pt.SetDeltaTime(deltaTime);
	}
	return 0;
}