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
		//HideCursor();
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

	void PrimTech::ToggleMouse()
	{
		if (m_mouseLocked) ShowCursor();
		else HideCursor();
		m_mouseLocked = !m_mouseLocked;
	}

	void PrimTech::Update(float& dt)
	{
		if (m_mouseLocked)
		{
			RECT Rect;
			GetWindowRect(m_window.getHWND(), &Rect);

			RECT rec = {};
			GetWindowRect(m_window.getHWND(), &rec);
			m_windowPos.x = rec.left;
			m_windowPos.y = rec.top;

			SetCursorPos(m_windowPos.x + (m_window.getWinWidth() / 2), m_windowPos.y + (m_window.getWinHeight() / 2));
		}
	}

	void PrimTech::HideCursor()
	{
		m_mouseLocked = true;
		while (::ShowCursor(false) >= 0);
	}

	void PrimTech::ShowCursor()
	{
		m_mouseLocked = false;
		while (::ShowCursor(true) < 0);
	}

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

