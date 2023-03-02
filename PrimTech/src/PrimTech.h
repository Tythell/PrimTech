#pragma once
#include "Windows/WindowWrap.h"
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
		void Update(float& dt);
		void HideCursor();
		void ShowCursor();
		Window m_window;
		DX11Renderer* mp_dxrenderer = nullptr;

		CameraHandler m_cams;

		KeyboardHandler m_kb;
		float m_playerSpeed;
		const unsigned char m_optionkey = Key::ESCAPE;
		const unsigned char m_camlockKey = Key::Q;
		d::XMINT2 m_windowPos;

		Entity testEntity;
		Entity testEntity2;
		Entity testEntity3;

		Entity m_tfFloor;
	};
};