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

		

		SetUpScriptEnviroment();
		LuaScript::SetLuaState(m_luaEngine.GetLuaState());

		/*pt::Entity& ent0 = pt::Entity::Create();
		pt::Camera* devCam = ent0.AddComponent<pt::Camera>();
		devCam->UpdateView(ent0.Transform());
		devCam->SetPerspective(80.f, (float)width / (float)height, 0.1f, 100.f);
		ent0.SetPosition(0.f, 1.f, -2.f);*/

		mp_dxrenderer = new Renderer(m_window);
		
		ID3D11Device* devi = mp_dxrenderer->GetDevice();
		pt::PhysicsBody::SetPtrs(&m_physHandler);
#ifdef PHYSDEBUG
		m_physHandler.EnableDebugDrawing(devi, true);

		mp_dxrenderer->SetDebugRenderer(&m_physHandler);
#endif // PHYSDEBUG
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
		m_physHandler.Update(dt);
		
		if (m_playing)
		{
			m_luaEngine.UpdateDeltaTime(dt);
			std::vector<LuaScript>& scripts = ComponentHandler::GetComponentArray<LuaScript>();
			uint numScripts = ComponentHandler::GetNoOfUsedComponents<LuaScript>();
			for (int i = 0; i < numScripts; i++)
			{
				m_luaEngine.ChangeCurrentLuaEnt(scripts[i].EntId());
				scripts[i].Execute("OnTick");
			}
		}
		
		std::vector<pt::AABBComp>& aabbs = ComponentHandler::GetComponentArray<pt::AABBComp>();

		//for (int i = 0; i < aabbs.size(); i++)
		//{
		//	pt::AABBComp& comp = aabbs[i];
		//	aabbs[i].EntId();
		//	pt::TransformComp& transform = ComponentHandler::GetComponentByIndex<pt::TransformComp>(aabbs[i].EntId());

		//	aabbs[i].Update(transform);
		//}

		//// FIXME naive and slow solution, has to be optimised in the future
		//for (int i = 0; i < aabbs.size(); i++)
		//	for (int j = 0; j < aabbs.size(); j++)
		//		if (i != j && aabbs[i].Intersects(aabbs[j]))
		//		{
		//			aabbs[i].SetIsIntersecting(true);
		//			aabbs[j].SetIsIntersecting(true);
		//		}


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

		std::vector<pt::Light>& r_lights = ComponentHandler::GetComponentArray<pt::Light>();
		uint numLights = ComponentHandler::GetNoOfUsedComponents<pt::Light>();
		//uint numLights = (uint)r_lights.size();
		for (int i = 0; i < numLights; i++)
		{
			r_lights[i].Update(pt::Entity::GetEntity(r_lights[i].EntId()).Transform());
		}
	}

	void PrimTech::SetUpScriptEnviroment()
	{
		int typeTable = m_luaEngine.CreateLuaCompTable();
		//m_luaEngine.AddTypeFunc()

		int transformTable = m_luaEngine.InitCompType("Transform", ec_transform, typeTable);

		int meshTable = m_luaEngine.InitCompType("MeshRef", ec_meshRef, typeTable);
		m_luaEngine.AddTypeFunc(MeshRef::Lua_ChangeModel, meshTable, "ChangeMesh");

		int camTable = m_luaEngine.InitCompType("Camera", ec_cam, typeTable);
		int lightTable = m_luaEngine.InitCompType("Light", ec_light, typeTable);
		int physTable = m_luaEngine.InitCompType("PhysBody", ec_rigidBodies, typeTable);


		m_entTableIdx = m_luaEngine.InitEntTable();
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

	void PrimTech::SetCamera(uint idx)
	{
		mp_dxrenderer->SetActiveCam(idx);
	}

	bool PrimTech::TogglePlay(char b)
	{
		if (b == 2)
		{
			m_playing = !m_playing;
		}
		else
			m_playing = (bool)b;

		return m_playing;
	}

	void PrimTech::Run()
	{
		float dtf = (float)m_deltaTime;
		Update(dtf);
		mp_dxrenderer->Render((float)m_deltaTime);
		m_isOpen = m_window.processMsg();
	}
	void PrimTech::ExecuteOnStart(pt::LuaScript* pScript)
	{
		if (!pScript)
		{
			std::vector<LuaScript>& scripts = ComponentHandler::GetComponentArray<LuaScript>();
			uint numScripts = ComponentHandler::GetNoOfUsedComponents<LuaScript>();
			for (int i = 0; i < numScripts; i++)
			{
				m_luaEngine.ChangeCurrentLuaEnt(scripts[i].EntId());
				scripts[i].Execute("OnStart");
			}
		}
		else
		{
			m_luaEngine.ChangeCurrentLuaEnt(pScript->EntId());
			pScript->Execute("OnStart");
		}

	}
	bool PrimTech::IsOpen() const
	{
		return m_isOpen;
	}
}

