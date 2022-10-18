#pragma once
#include "WindowWrap.h"
#include <omp.h>
#include <thread>
#include "Comlib/Comlib.h"


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

		//Testing Comlib
		RingBuffer m_consumerBuffer;

		Window m_window;
		DX11Addon* mp_gApi = nullptr;
		std::vector<Camera> m_cam3d;
		KeyboardHandler m_kb;
		float m_playerSpeed;
		unsigned char m_shutDownKey = Key::ESCAPE;
		const unsigned char m_camlockKey = Key::Q;
		d::XMINT2 m_windowPos;
	};

};

