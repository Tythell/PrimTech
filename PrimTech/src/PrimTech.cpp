#include"pch.h"
#include "PrimTech.h"
#include "Graphics/DX11Wrapper.h"
#include"ecs/Entity.h"

using namespace PrimtTech;

namespace pt
{
	PrimTech::PrimTech() :
		m_playerSpeed(5.f)
	{
		HideCursor();
	}

	PrimTech::~PrimTech()
	{
		if (mp_dxrenderer)
			delete mp_dxrenderer;
	}

	void PrimTech::LoadPak(std::string path)
	{
		// TODO CREATE ASSETLOADER WITH PACKAGING
		THROW_POPUP_ERROR(false, "asset-pak loading not implemented yet, load assets manually using PrimtTech::ResourceHandler to reserve assets until it's ready");
		ResourceHandler::LoadPak(path);
	}

	void PrimTech::Init(LPCWSTR windowName, HINSTANCE hInstance, std::wstring windowClass, unsigned int width, unsigned int height)
	{
		m_window.init(windowName, hInstance, windowClass, width, height);
		m_window.SetInputP(m_kb);

		//m_cellCam.SetPosition(0.f, 0.f, -1.f);
		//m_cam3d.SetPerspective(80, (float)width / (float)height, 0.1f, 100.f);
		//m_cam3d.SetPosition(2.f, 0, -3.f);
		//m_camera.SetOrtographic(width, height, 0, 1);

		m_cams.Init({ (int)width, (int)height }, NULL);

		m_cams.GetCameraAdress(0)->SetRotationSpeed(0.001f);
		mp_dxrenderer = new DX11Renderer(m_window, m_cams);
		mp_dxrenderer->SetInputP(m_kb);

		//ComponentHandler::ReserveMemory<MeshRef>(6);
	}

	void PrimTech::Update(float& dt)
	{
		sm::Vector3 move = { 0.f,0.f,0.f };
		//bool canMove = MouseHandler::GetIsMouseDown(eRIGHTCLICK);
		static bool canMove = true;
		/*if (m_kb.IsKeyDown(Key::A))
			move += m_cams.GetCurrentCamera()->GetLeftVector();
		if (m_kb.IsKeyDown(Key::D))
			move += -m_cams.GetCurrentCamera()->GetLeftVector();
		if (m_kb.IsKeyDown(Key::W))
			move += m_cams.GetCurrentCamera()->GetForwardVectorNoY();
		if (m_kb.IsKeyDown(Key::S))
			move += -m_cams.GetCurrentCamera()->GetForwardVectorNoY();
		if (m_kb.IsKeyDown(Key::SPACE))
			move += {0.f, 1.f, 0.f};
		if (m_kb.IsKeyDown(Key::SHIFT))
			move += {0.f, -1.f, 0.f};
		move.Normalize();
		move *= m_playerSpeed * dt;*/

		static bool isclick = false;

		if (canMove && m_window.GetIsFocused())
		{
			SetCursorPos(m_windowPos.x + (m_window.getWinWidth() / 2), m_windowPos.y + (m_window.getWinHeight() / 2));
		}
		m_cams.GetCurrentCamera()->Move(move);

		mp_dxrenderer->SetCanMove(canMove);

		if (!m_kb.IsKeyDown(m_camlockKey))
			isclick = false;
		else if (m_kb.IsKeyDown(m_camlockKey) && !isclick)
		{
			isclick = true;
			canMove = !canMove;

			if (!canMove) ShowCursor();
			else
			{
				HideCursor();
				RECT rec = {};
				GetWindowRect(m_window.getHWND(), &rec);
				m_windowPos.x = rec.left;
				m_windowPos.y = rec.top;
			}
		}
		while (!MouseHandler::BufferIsEmpty())
		{
			MouseEvent me = MouseHandler::ReadEvent();
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE && canMove)
			{
				m_cams.GetCurrentCamera()->Rotate((float)me.GetPosition().y, (float)me.GetPosition().x, 0.f);
			}
			else if (me.GetType() == MouseEvent::EventType::eSCROLLUP && canMove)
				m_cams.GetCurrentCamera()->Offset(0.f, 0.f, -0.5f);
			else if (me.GetType() == MouseEvent::EventType::eSCROLLDOWN && canMove)
				m_cams.GetCurrentCamera()->Offset(0.f, 0.f, 0.5f);
			else if (me.GetType() == MouseEvent::EventType::eLEFTCLICK)
			{
				//float mouseX = (float)me.GetPosition().x;
				//float mouseY = (float)me.GetPosition().y;

				//float winWidth = m_window.getWinWidth();
				//float winHeight = m_window.getWinHeight();

				//float x = (2.f * mouseX) / winWidth - 1.f;
				//float y = 1.f - (2.f * mouseY) / winHeight;

				//sm::Vector4 clipRay(x, y, -1.f, 1.f);

				//sm::Vector4 eyeRay = XMVector4Transform(clipRay, d::XMMatrixInverse(nullptr, m_cams.GetCurrentCamera()->GetProjM()));

				//eyeRay = sm::Vector4(eyeRay.x, eyeRay.y, 1.f, 0.f);

				//sm::Vector4 worldRay = XMVector4Transform(eyeRay, d::XMMatrixInverse(nullptr, m_cams.GetCurrentCamera()->GetViewM()));

				//sm::Vector3 normRay(worldRay.x, worldRay.y, worldRay.z);

				//normRay.Normalize();

				//mp_dxrenderer->Click(normRay);
			}
		}
#ifdef _DEBUG
		if (m_kb.IsKeyDown(m_optionkey))
			m_window.ShutDown();
#endif // _DEBUG


	}

	void PrimTech::HideCursor()
	{
		while (::ShowCursor(false) >= 0);
	}

	void PrimTech::ShowCursor()
	{
		while (::ShowCursor(true) < 0);
	}

	//void ThreadFunc(DX11Renderer* dx, bool* running)
	//{
	//	double start = 0, deltatime = 0;

	//	while (*running)
	//	{
	//		start = omp_get_wtime();
	//		//float dt = .5f;
	//		float dt = (float)deltatime;
	//		dx->CalculateFps(dt);
	//		dx->UpdateScene(dt);
	//		
	//		dx->Render(dt);

	//		deltatime = omp_get_wtime() - start;
	//	}

	//}

	void PrimTech::Run()
	{
		float dtf = (float)m_deltaTime;
		Update(dtf);
		mp_dxrenderer->UpdateScene((m_deltaTime));
		mp_dxrenderer->Render((float)m_deltaTime);
		m_isOpen = m_window.processMsg();
	}
	bool PrimTech::IsOpen() const
	{
		return m_isOpen;
	}
}

