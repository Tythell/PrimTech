#pragma once
#include "Windows/WindowWrap.h"
#include <omp.h>
#include <thread>
#include"Physics/PhysicsHandler.h"
#include"scripting/LuaHandler.h"

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

		void ExecuteOnStart(pt::LuaScript* pScript = nullptr);
		
		bool IsOpen() const;
		void SetDeltaTime(double& dt) { m_deltaTime = dt; };
		double GetDeltaTime() const { return m_deltaTime; };
		PrimtTech::Renderer* GetRenderer() { return mp_dxrenderer; }
		void Close() { m_window.ShutDown(); };
		void ToggleMouse();
		void HideCursor();
		void ShowCursor();
		PrimtTech::PhysicsHandler m_physHandler;
		
		void SetCamera(uint idx);
		bool TogglePlay(char b = 2);
		void SetAlwaysOnTop(bool b)
		{
			if (b)
				SetWindowPos(m_window.getHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			else
				SetWindowPos(m_window.getHWND(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			
		}

		void CreateImGuiWindow(PrimtTech::ImGuiWindowFunc func, void* args, bool* openClose = (bool*)0);
		
	private:
		void Update(float dt);
		void SetUpScriptEnviroment();
		
		std::wstring m_windowName;
		PrimtTech::Window m_window;
		PrimtTech::Renderer* mp_dxrenderer = nullptr;
		PrimtTech::LuaEngine m_luaEngine;
		PrimtTech::ImGuiHandler* m_pGui = nullptr;

		float m_playerSpeed;
		double m_deltaTime = 0.0;
		bool m_isOpen = true;
		bool m_playing = false;

		bool m_mouseLocked = false;
		uint m_activeCamera = 1;
		int m_entTableIdx = 0;

		int2 m_windowPos = int2(0,0);
	};
};