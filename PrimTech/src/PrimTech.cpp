#include "PrimTech.h"
#include "Graphics/DX11Wrapper.h"
#include <string>

namespace pt
{
	PrimTech::PrimTech() :
		m_playerSpeed(5.f), m_consumerBuffer(L"Filemap", TYPE::READ, 150)
	{
		//m_cam3d.resize(1);
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

		m_window.SetInputP(m_kb);

		//m_cellCam.SetPosition(0.f, 0.f, -1.f);
		m_cams.Init({ (int)width, (int)height });
		Camera* pCam2 = m_cams.CreatePerspectiveCamera("TestCamera", 80, (float)width / (float)height, 0.1f, 100.f);
		pCam2->SetPosition(2.f, .2f, -3.f);
		//pCam2->SetPosition(2.f, 0.5f, -3.f);
		//m_cam3d[0].SetPosition(2.f, 0, -3.f);

		mp_gApi = new DX11Addon(m_window, m_cams);
		mp_gApi->SetInputP(m_kb);
	}

	void PrimTech::Update(const float& dt)
	{
		if (m_kb.IsKeyDown(m_shutDownKey))
			m_window.ShutDown();
		char* message = nullptr;
		SectionHeader* header;

		bool recievedSuccess = m_consumerBuffer.Recieve(message, header);
		if (recievedSuccess)
		{
			switch (header->header)
			{
			case Headers::MESSAGE:
			{
				OutputDebugStringA(message);
				break;
			}
			case Headers::eCAMERACREATE:
			{
				CameraCreate camMessage;
				memcpy((char*)&camMessage, message, header->msgLen);
				m_cams.CreatePerspectiveCamera(camMessage.cameraName, camMessage.fov, 
					(float)m_window.getWinHeight() / (float)m_window.getWinHeight(), 
					0.1f, 100.f);
				break;
			}
			case Headers::eCAMERAMOVE:
			{
				CameraMove m;
				memcpy((char*)&m, message, header->msgLen);
				Camera* pcam = m_cams.GetCameraAdress(std::string(m.cameraName));
				
				sm::Matrix mat = *reinterpret_cast<sm::Matrix*>(m.matrix);
				
				pcam->OverrideView(mat);
				break;
			}
			case Headers::eLOADTEXTURE:
			{

				break;
			}
			case Headers::eNAMECHANGE:
			{

				break;
			}
			case Headers::eNEWMESH:
			{

				break;
			}
			case Headers::eNEWTOPOLOGY:
			{

				break;
			}
			case Headers::eOBJECTDRAG:
			{

				break;
			}
			case Headers::eVERTEXDRAG:
			{

				break;
			}
			default:
				break;
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

