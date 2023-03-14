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
		m_windowName = windowName;
		m_window.init(windowName, hInstance, windowClass, width, height);
		m_window.SetInputP(m_kb);

		mp_dxrenderer = new DX11Renderer(m_window);
		mp_dxrenderer->SetInputP(m_kb);

		//ComponentHandler::ReserveMemory<MeshRef>(6);
	}

	void PrimTech::ToggleMouse()
	{
		if (m_mouseLocked) ShowCursor();
		else HideCursor();
		m_mouseLocked = !m_mouseLocked;
	}

	void PrimTech::Update(float dt)
	{
		static float timer = 0.f;
		timer += dt;
		if (timer >= 1.f)
		{
			timer = 0.f;
			int fps = 1.f / dt;
			std::wstring extendedWinName = m_windowName + L"  -  FPS: " + std::to_wstring(fps);

			::SetWindowTextW(m_window.getHWND(), extendedWinName.c_str());
		}

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

