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
	
	m_cellCam.SetPosition(0.f, 0.f, -1.f);
	m_cam3d.SetPerspective(70, (float)width / (float)height, 0.1f, 100.f);
	m_cam3d.SetPosition(0, 0, -1);
	//m_camera.SetOrtographic(width, height, 0, 1);

	mp_gApi = new DX11Addon(m_window, m_cam3d);
	mp_gApi->SetInputP(m_kb);
}

void PrimTech::Update(double& dt)
{
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