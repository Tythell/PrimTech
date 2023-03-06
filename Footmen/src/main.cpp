#include"Scene.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	pt::PrimTech pt;

	PrimtTech::ResourceHandler::ReserveMeshMemory(10);
	PrimtTech::ResourceHandler::ReserveMaterialMemory(8);
	pt.Init(L"Editor", hInstance, L"wndc", 1700, 1000);
	//PrimtTech::ImGuiHandler* gui = pt.GetRenderer()->GetGuiHandlerP();




	int nu = 5;
	//gui->AddWindowFunc(ImGuiWinTest, (int*)&nu);

	PrimtTech::ResourceHandler::AddMesh("Assets/models/Slime.fbx");

	Scene scene(pt.GetRenderer()->GetGuiHandlerP());

	double start = 0, deltaTime = 0;

	start = omp_get_wtime();

	bool test = true;

	double timer = 0;

	while (pt.IsOpen())
	{
		start = omp_get_wtime();
		pt.Run();
		scene.Update();

		deltaTime = omp_get_wtime() - start;
		pt.SetDeltaTime(deltaTime);
	}
	return 0;
}