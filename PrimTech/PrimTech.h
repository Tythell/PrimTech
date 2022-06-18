#pragma once
#include "WindowWrap.h"
#include <omp.h>
#include <thread>

namespace pt
{
	class PrimTech
	{
	public:
		PrimTech();
		~PrimTech();
		void Init(LPCWSTR windowName, HINSTANCE hInstance, std::wstring windowClass, unsigned int width, unsigned int height);
		void Run();
	private:
		void Update(const float& dt);
		void HideCursor();
		void ShowCursor();
		Window m_window;
		DX11Addon* mp_gApi = nullptr;
		//Camera m_cellCam;
		Camera m_cam3d;
		KeyboardHandler m_kb;
		//MouseHandler m_mouse;
		float m_playerSpeed;
	};

};

