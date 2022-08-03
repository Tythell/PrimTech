#include "PrimTech.h"
#include "Graphics/DX11Wrapper.h"
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
		if (mp_gApi) 
			delete mp_gApi;
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
		//bool canMove = MouseHandler::GetIsMouseDown(eRIGHTCLICK);
		static bool canMove = true;
		if (m_kb.IsKeyDown(Key::A)) move += m_cam3d.GetLeftVector();
		if (m_kb.IsKeyDown(Key::D)) move += -m_cam3d.GetLeftVector();
		if (m_kb.IsKeyDown(Key::W)) move += m_cam3d.GetForwardVectorNoY();
		if (m_kb.IsKeyDown(Key::S)) move += -m_cam3d.GetForwardVectorNoY();
		if (m_kb.IsKeyDown(Key::SPACE)) move += {0.f, 1.f, 0.f};
		if (m_kb.IsKeyDown(Key::SHIFT)) move += {0.f, -1.f, 0.f};
		move.Normalize();
		move *= m_playerSpeed * dt;
		
		static bool isclick = false;

		if (canMove && m_window.GetIsFocused())
		{
			SetCursorPos(m_windowPos.x + (m_window.getWinWidth() / 2),m_windowPos.y + (m_window.getWinHeight() / 2));
		}
		m_cam3d.Move(move);
		
		mp_gApi->SetCanMove(canMove);
		
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
				m_cam3d.Rotate(me.GetPosition().y, me.GetPosition().x, 0.f);
			}
			else if (me.GetType() == MouseEvent::EventType::eSCROLLUP && canMove)
				m_cam3d.Offset(0.f, 0.f, -0.5f);
			else if (me.GetType() == MouseEvent::EventType::eSCROLLDOWN && canMove)
				m_cam3d.Offset(0.f, 0.f, 0.5f);
			else if (me.GetType() == MouseEvent::EventType::eLEFTCLICK)
			{
				float mouseX = me.GetPosition().x;
				float mouseY = me.GetPosition().y;

				float winWidth = m_window.getWinWidth();
				float winHeight = m_window.getWinHeight();

				float x = (2.f * mouseX) / winWidth - 1.f;
				float y = 1.f - (2.f * mouseY) / winHeight;

				sm::Vector4 clipRay(x, y, -1.f, 1.f);

				sm::Vector4 eyeRay = XMVector4Transform(clipRay, d::XMMatrixInverse(nullptr, m_cam3d.GetProjM()));

				eyeRay = sm::Vector4(eyeRay.x, eyeRay.y, 1.f, 0.f);

				sm::Vector4 worldRay = XMVector4Transform(eyeRay, XMMatrixInverse(nullptr, m_cam3d.GetViewM()));

				sm::Vector3 normRay(worldRay.x, worldRay.y, worldRay.z);

				normRay.Normalize();

				mp_gApi->Click(normRay);
			}
		}
		if (m_kb.IsKeyDown(m_shutDownKey))
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

