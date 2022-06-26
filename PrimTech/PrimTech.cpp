#include "PrimTech.h"
#include <string>

namespace pt
{
	PrimTech::PrimTech() :
		m_playerSpeed(5.f)
	{
		HideCursor();
	}

	PrimTech::~PrimTech()
	{
		if (mp_gApi) delete mp_gApi;
	}

	void PrimTech::Init(LPCWSTR windowName, HINSTANCE hInstance, std::wstring windowClass, unsigned int width, unsigned int height)
	{
		m_window.init(windowName, hInstance, windowClass, width, height);
		m_window.SetInputP(m_kb);

		//m_cellCam.SetPosition(0.f, 0.f, -1.f);
		m_cam3d.SetPerspective(80, (float)width / (float)height, 0.1f, 100.f);
		m_cam3d.SetPosition(2.f, 0, -3.f);
		//m_camera.SetOrtographic(width, height, 0, 1);

		mp_gApi = new DX11Addon(m_window, m_cam3d);
		mp_gApi->SetInputP(m_kb);
	}

	void PrimTech::Update(const float& dt)
	{
		m_cam3d.SetRotationSpeed(0.001f);
		sm::Vector3 move = { 0.f,0.f,0.f };
		static bool canMove = true;
		if (m_kb.IsKeyDown(Key::A)) move += m_cam3d.GetLeftVector();
		if (m_kb.IsKeyDown(Key::D)) move += -m_cam3d.GetLeftVector();
		if (m_kb.IsKeyDown(Key::W)) move += m_cam3d.GetForwardVector();
		if (m_kb.IsKeyDown(Key::S)) move += -m_cam3d.GetForwardVector();
		if (m_kb.IsKeyDown(Key::SPACE)) move += {0.f, 1.f, 0.f};
		if (m_kb.IsKeyDown(Key::SHIFT)) move += {0.f, -1.f, 0.f};
		move *= m_playerSpeed * dt;
		m_cam3d.Move(move);
		static bool isclick = false;

		if (canMove)
			SetCursorPos(m_window.getWinWidth() / 2, m_window.getWinHeight() / 2);


		if (m_kb.IsKeyDown(Key::Q) && !isclick)
		{
			isclick = true;
			canMove = !canMove;

			if (!canMove) ShowCursor();
			else HideCursor();
		}
		if (!m_kb.IsKeyDown(Key::Q)) isclick = false;

		while (!MouseHandler::BufferIsEmpty())
		{
			MouseEvent me = MouseHandler::ReadEvent();
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE && canMove)
			{
				m_cam3d.Rotate(me.GetPosition().y, me.GetPosition().x, 0.f);
			}
			else if (me.GetType() == MouseEvent::EventType::eSCROLLUP)
				m_cam3d.Offset(0.f, 0.f, -0.5f);
			else if (me.GetType() == MouseEvent::EventType::eSCROLLDOWN)
				m_cam3d.Offset(0.f, 0.f, 0.5f);
		}
		if (m_kb.IsKeyDown(Key::ESCAPE))
			m_window.ShutDown();
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
			Update(deltatime);
			mp_gApi->CalculateFps(deltatime);
			mp_gApi->UpdateScene(deltatime);
			mp_gApi->Render(deltatime);
			deltatime = omp_get_wtime() - start;

			running = m_window.processMsg();
		}

	}
}

