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

		CameraHandler m_cams;

		KeyboardHandler m_kb;
		float m_playerSpeed;
		unsigned char m_shutDownKey = Key::ESCAPE;
		const unsigned char m_camlockKey = Key::Q;
		d::XMINT2 m_windowPos;
	};
};