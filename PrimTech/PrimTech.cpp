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

	m_camera.SetPosition(0.f, 0.f, -1.f);
	//m_camera.SetOrtographic(width, height, 0, 1);

	mp_gApi = new DX11Addon(m_window, m_camera);
}

void PrimTech::Update(double& dt)
{
}

void PrimTech::Run()
{
	double start = 0, deltatime = 0;
	while (m_window.processMsg())
	{
		start = omp_get_wtime();
		Update(deltatime);
		mp_gApi->Render(deltatime);
		deltatime = omp_get_wtime() - start;
	}
}

