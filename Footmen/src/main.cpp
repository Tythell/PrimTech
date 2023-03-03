#include"PrimTech.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	pt::PrimTech pt;

	ResourceHandler::ReserveMeshMemory(10);
	ResourceHandler::ReserveMaterialMemory(8);
	pt.Init(L"shat app", hInstance, L"wndclass", 1800, 1000);



	ResourceHandler::AddMesh("Assets/models/Slime.fbx");




	Entity ent;
	ent.AddComponent<MeshRef>()->Init("Slime.fbx");


	pt.Run();
	return 0;
}