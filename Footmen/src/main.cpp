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

	d::XMINT2 windowRes(1700, 900);
	pt.Init(L"Editor", hInstance, L"wndc", windowRes.x, windowRes.y);

	Editor scene(pt.GetRenderer(), windowRes);

	double start = 0, deltaTime = 0;
	start = omp_get_wtime();

	while (pt.IsOpen())
	{
		start = omp_get_wtime();
		pt.Run();
		scene.Update((float)deltaTime);

		while (!scene.m_msgQueue.empty())
		{
			Editor::Messages postMsg = scene.m_msgQueue.front();
			switch (postMsg)
			{
			case Editor::Messages::eQuit:
				pt.Close();
				break;
			case Editor::Messages::eToggleMouse:
				pt.ToggleMouse();
				break;
			case Editor::Messages::eHideMouse:
				pt.HideCursor();
				break;
			case Editor::Messages::eShowMouse:
				pt.ShowCursor();
				break;
			default:
				break;
			}
			scene.m_msgQueue.pop();
		}

		deltaTime = omp_get_wtime() - start;
		pt.SetDeltaTime(deltaTime);
	}
	return 0;
}