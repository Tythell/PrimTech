#include "PrimTech.h"
#include "Graphics/DX11Wrapper.h"
#include <string>

namespace pt
{
	PrimTech::PrimTech() :
		m_playerSpeed(5.f)
	{
		m_cam3d.resize(1);
		//HideCursor();
	}

	PrimTech::~PrimTech()
	{
		if (mp_gApi) 
			delete mp_gApi;
	}

	void PrimTech::Init(LPCWSTR windowName, HINSTANCE hInstance, std::wstring windowClass, unsigned int width, unsigned int height)
	{
		m_window.init(windowName, hInstance, windowClass, width, height);

		consumerBuffer = new Comlib(L"Filemap", 150, Consumer);

		m_window.SetInputP(m_kb);

		//m_cellCam.SetPosition(0.f, 0.f, -1.f);
		m_cam3d[0].SetPerspective(80, (float)width / (float)height, 0.1f, 100.f);
		m_cam3d[0].SetPosition(2.f, 0, -3.f);
		//m_camera.SetOrtographic(width, height, 0, 1);

		mp_gApi = new DX11Addon(m_window, m_cam3d[0]);
		mp_gApi->SetInputP(m_kb);
	}

	void PrimTech::Update(const float& dt)
	{
		if (m_kb.IsKeyDown(m_shutDownKey))
			m_window.ShutDown();

		while (consumerBuffer->Recieve(msg, mainHeader))
		{
			if (mainHeader->header == MESSAGE)
			{
				OutputDebugStringW(L"We got a MESSAGE message.");

				MessageHeader* message = new MessageHeader;
				memcpy(message, msg, sizeof(MessageHeader));

				const size_t size = strlen(message->message);
				wchar_t* recievedMsg = new wchar_t[size];
				mbstowcs(recievedMsg, message->message, size);
				
				sm::Vector3 p(message->position[0], message->position[1],message->position[3]);
				std::wstring pos = L"\nPosition " + std::to_wstring((int)message->position[0]) + L" " 
					+ std::to_wstring((int)message->position[1]) + L" " 
					+ std::to_wstring((int)message->position[2]);
				
				OutputDebugStringW(L"\n\n");
				OutputDebugStringW(recievedMsg);
				OutputDebugStringW(pos.c_str());
				delete message;
				message = nullptr;
			}
		}
	}

	void PrimTech::HideCursor()
	{
		while(::ShowCursor(false) >= 0);
	}

	void PrimTech::ShowCursor()
	{
		while(::ShowCursor(true) < 0);
	}

	void PrimTech::Run()
	{
		double start = 0, deltatime = 0;

		bool running = true;
		while (running)
		{
			start = omp_get_wtime();
			Update((float)deltatime);
			mp_gApi->CalculateFps((float)deltatime);
			mp_gApi->UpdateScene((float)deltatime);
			mp_gApi->Render((float)deltatime);
			deltatime = omp_get_wtime() - start;

			running = m_window.processMsg();
		}

	}
}

