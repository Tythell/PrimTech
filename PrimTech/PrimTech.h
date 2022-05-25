#pragma once
#include "WindowWrap.h"
#include <omp.h>
#include <thread>

class PrimTech
{
public:
	PrimTech();
	~PrimTech();
	void Init(LPCWSTR windowName, HINSTANCE hInstance, std::wstring windowClass, unsigned int width, unsigned int height);
	void Run();
	void multiThreadTest();
private:
	void Update(double& dt);
	Window m_window;
	DX11Addon* mp_gApi;
	Camera m_camera;
	KeyboardHandler m_kb;
};

