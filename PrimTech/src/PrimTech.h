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

		void LoadPak(std::string path);

		void Init(LPCWSTR windowName, HINSTANCE hInstance, std::wstring windowClass, unsigned int width, unsigned int height);
		void Run();
		
		bool IsOpen() const;
		void SetDeltaTime(double& dt) { m_deltaTime = dt; };
		double GetDeltaTime() const { return m_deltaTime; };
		PrimtTech::DX11Renderer* GetRenderer() { return mp_dxrenderer; }
	private:
		void Update(float& dt);
		void HideCursor();
		void ShowCursor();
		PrimtTech::Window m_window;
		PrimtTech::DX11Renderer* mp_dxrenderer = nullptr;
		PrimtTech::CameraHandler m_cams;

		KeyboardHandler m_kb;
		float m_playerSpeed;
		double m_deltaTime = 0.0;
		bool m_isOpen = true;

		const unsigned char m_optionkey = Key::ESCAPE;
		const unsigned char m_camlockKey = Key::Q;
		d::XMINT2 m_windowPos;
	};
};