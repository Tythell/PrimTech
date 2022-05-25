#include "PrimTech.h"

PrimTech::PrimTech()
{
}

PrimTech::~PrimTech()
{
	if(mp_gApi) delete mp_gApi;
}

void PrimTech::Init(LPCWSTR windowName, HINSTANCE hInstance, std::wstring windowClass, unsigned int width, unsigned int height)
{
	m_window.init(windowName, hInstance, windowClass, width, height);
	m_window.SetInputP(m_kb);
	
	m_camera.SetPosition(0.f, 0.f, -1.f);
	//m_camera.SetOrtographic(width, height, 0, 1);

	mp_gApi = new DX11Addon(m_window, m_camera);
	mp_gApi->SetInputP(m_kb);
}

void PrimTech::Update(double& dt)
{
}

static bool s_finnished = false;
void func()
{
	while (!s_finnished)
	{

	}
}

void PrimTech::Run()
{
	double start = 0, deltatime = 0;

	bool running = true;
	while (running)
	{
		start = omp_get_wtime();
		Update(deltatime);
		mp_gApi->Render();
		deltatime = omp_get_wtime() - start;

		running = m_window.processMsg();
	}

}