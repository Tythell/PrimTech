#include "Scene.h"
#include "Serializer/Serializer.h"
#include"WindowFuncs.h"
#include <sstream>

namespace ig = ImGui;

Editor::Editor(d::XMINT2 windowRes, HINSTANCE hInstance)
{
	PrimtTech::ResourceHandler::ReserveMeshMemory(15);
	PrimtTech::ResourceHandler::ReserveMaterialMemory(8);

	m_primtech.Init(L"Editor", hInstance, L"wndc", windowRes.x, windowRes.y);

	m_renderer = m_primtech.GetRenderer();

	PrimtTech::ResourceHandler::AddMesh("Assets/models/cube.txt");
	PrimtTech::ResourceHandler::AddMesh("Assets/models/gunter.obj");
	PrimtTech::ResourceHandler::AddMesh("Assets/models/scuffball.obj");
	//PrimtTech::ResourceHandler::AddMesh("Assets/models/Slime.fbx");
	PrimtTech::ResourceHandler::AddMesh("Assets/models/scaledplane.obj");
	PrimtTech::ResourceHandler::AddMaterial("DefaultMaterial");

	m_renderer->SetImGuiHandler(m_pGui);

	uint u = 0;
	m_entlist.console.m_showWin.resize(7, { true, "" });
	m_pGui.AddWindowFunc(Gui_ImGuiDemo, NULL, &m_entlist.console.m_showWin[u].first);
	m_entlist.console.m_showWin[u].second = "imguiDemo";
	m_entlist.console.m_showWin[u++].first = false;
	m_pGui.AddWindowFunc(Gui_EntList, &m_entlist, &m_entlist.console.m_showWin[u].first);
	m_entlist.console.m_showWin[u++].second = "entlist";
	m_pGui.AddWindowFunc(Gui_AssetList, &m_entlist, &m_entlist.console.m_showWin[u].first);
	m_entlist.console.m_showWin[u++].second = "assetlist";
	m_pGui.AddWindowFunc(Gui_Console, &m_entlist.console, &m_entlist.console.m_showWin[u].first);
	m_entlist.console.m_showWin[u++].second = "console";
	m_pGui.AddWindowFunc(Gui_MaterialProperties, &m_entlist.m_selectedMaterial, &m_entlist.console.m_showWin[u].first);
	m_entlist.console.m_showWin[u++].second = "mtrlpropers";
	m_pGui.AddWindowFunc(ImguiDebug, m_pGui.GetVarPtrs(), &m_entlist.console.m_showWin[u].first);
	m_entlist.console.m_showWin[u++].second = "debugSettings";


	m_pGui.AddWindowFunc(Gui_menubar, &m_entlist.console, &m_entlist.console.m_showWin[u].first);
	m_entlist.console.m_showWin[u++].second = "Menu_Bar";

	pt::Entity::ReserveEnts(10);

	m_entlist.winWidth = windowRes.x;
	m_entlist.winHeight = windowRes.y;

	pt::Light* pLight = pt::Entity::Create().AddComponent<pt::Light>();
	pLight->SetType(2);
	pLight->SetColor({ 1.f, 1.f, 1.f, .2f });

	pLight = pt::Entity::Create().AddComponent<pt::Light>();
	pLight->SetType(1);
	pLight->SetDirectionOffset({ 0.f, 1.f, 1.f, 1.f });
	pLight->SetColor({ 1.f, 1.f, 1.f, 1.f });

	pt::Entity& ent0 = pt::Entity::Create();
	ent0.AddComponent<pt::MeshRef>();
	ent0.Transform().SetPosition(0.f, 2.f, 0.f);
	pt::Entity& ent1 = pt::Entity::Create();
	ent1.Transform().SetPosition(0.f, -0.2f, 0.f);
	ent1.Transform().SetScale(10.f, 1.f, 10.f);
	ent1.AddComponent<pt::MeshRef>()->Init("scaledplane.obj");
}

Editor::~Editor()
{
}

void Editor::execCommand(std::string cmd)
{
	std::stringstream ss;
	ss.clear();

	ss << cmd;
	std::string argBuffer;
	ss >> argBuffer;

	if (argBuffer == "create")
	{
		ss >> argBuffer;
		if (argBuffer == "ent")
		{
			pt::Entity::Create();
		}
		else if (argBuffer == "material")
		{
			PrimtTech::ResourceHandler::AddMaterial("new material");
		}
		else if (argBuffer == "mesh")
		{
			std::string path = Dialogs::OpenFile("Model (*.obj, *.fbx)\0*.obj;*.txt;*.fbx;\0", "Assets\\models\\");
			if (!path.empty())
			{
				PrimtTech::ResourceHandler::AddMesh(path);
			}
		}
		else if (argBuffer == "lwtex") // lightwarp
		{
			ss >> argBuffer;
			m_renderer->SetLightWarp(argBuffer);
			//if (argBuffer == "load")
			//{
			//	ss >> argBuffer;
			//	
			//}
		}
	}
	else if (argBuffer == "comp")
	{
		ss >> argBuffer;
		if (StringHelper::IsNumber(argBuffer))
		{
			uint entId = atoi(argBuffer.c_str());
			ss >> argBuffer;
			pt::Entity& rEnt = pt::Entity::GetEntity(entId);
			bool edit = false;
			if (argBuffer == "edit")
			{
				edit = true;
				ss >> argBuffer;
			}

			if (argBuffer == "cam")
			{
				rEnt.AddComponent<pt::Camera>();
			}
			else if (argBuffer == "meshref")
			{
				rEnt.AddComponent<pt::MeshRef>();
			}
			else if (argBuffer == "aabb")
			{
				rEnt.AddComponent<pt::AABBComp>();
			}
			else if (argBuffer == "light")
			{
				rEnt.AddComponent<pt::Light>();
			}
			else if (argBuffer == "rigidbody")
			{
				ss >> argBuffer;
				rp::BodyType type = rp::BodyType(1);
				if (argBuffer == "static") type = rp::BodyType(0);
				if (argBuffer == "kinematic") type = rp::BodyType(1);
				if (argBuffer == "dynamic") type = rp::BodyType(2);

				pt::PhysicsBody* pRigidBody = rEnt.AddComponent<pt::PhysicsBody>();
				if (edit)
					pRigidBody->SetType(type);
				else
					pRigidBody->CreateRigidBody(rEnt.Transform());
			}

		}
	}
	else if (argBuffer == "select")
	{
		ss >> argBuffer;
		m_entlist.selected = StringHelper::IsNumber(argBuffer) ? atoi(argBuffer.c_str()) : -1;

	}
	else if (argBuffer == "exit")
	{
		m_msgQueue.push(Messages::eQuit);
	}
	else if (argBuffer == "mouse")
	{
		ss >> argBuffer;
		if (argBuffer == "hide")
			m_msgQueue.push(Messages::eHideMouse);
		else if (argBuffer == "show")
			m_msgQueue.push(Messages::eShowMouse);
		else if (argBuffer == "sense")
		{
			ss >> argBuffer;
			m_mouseSense = atof(argBuffer.c_str()) / 100.f;
		}
	}
	else if (argBuffer == "showwin")
	{
		ss >> argBuffer;
		if (StringHelper::IsNumber(argBuffer))
		{
			uint num = atoi(argBuffer.c_str());
			m_entlist.console.m_showWin[num].first = !m_entlist.console.m_showWin[num].first;
		}
		else
		{
			for (int i = 0; i < m_entlist.console.m_showWin.size(); i++)
			{
				if (m_entlist.console.m_showWin[i].second == argBuffer)
				{
					m_entlist.console.m_showWin[i].first = !m_entlist.console.m_showWin[i].first;
				}
			}
		}

	}
	else if (argBuffer == "scene")
	{
		ss >> argBuffer;
		if (argBuffer == "load")
		{
			std::string path = Dialogs::OpenFile("Scene (*.ptsc)\0*.ptsc;\0", "Scenes\\");
			if (!path.empty())
			{
				m_entlist.selected = -1;
				pt::Entity::Clear(1);

				pt::Entity::GetEntity(0).AddComponent<pt::Camera>()->SetPerspective(80.f, ((float)m_entlist.winWidth / (float)m_entlist.winHeight), 0.1f, 100.f);
				PrimtTech::Import(path, pt::Entity::GetAllEnts());
			}
		}
		else if (argBuffer == "export")
		{
			std::string path = Dialogs::SaveFile("Scene (*.ptsc)\0*.ptsc;\0", "Scenes\\");
			if (!path.empty())
			{
				if (StringHelper::GetExtension(path) != "ptsc")
					path += ".ptsc";
				PrimtTech::Export(path, pt::Entity::GetAllEnts());
			}
		}
		else if (argBuffer == "new")
		{
			m_entlist.selected = -1;
			pt::Entity::Clear(4);
			pt::Entity::GetEntity(0).AddComponent<pt::Camera>()->SetPerspective(80.f, ((float)m_entlist.winWidth / (float)m_entlist.winHeight), 0.1f, 100.f);
		}
	}
}

void Editor::Update(float deltatime)
{
	//  console
	if (!m_entlist.console.cmdQ.empty())
	{
		std::string cmd = m_entlist.console.cmdQ.front();
		execCommand(cmd);
		m_entlist.console.cmdQ.pop();
	}

	sm::Vector3 move = { 0.f,0.f,0.f };

	bool canMove = MouseHandler::GetIsMouseDown(eRIGHTCLICK);

	pt::TransformComp* pDevTransform = &PrimtTech::ComponentHandler::GetComponentByIndex<pt::TransformComp>(0);
	pt::Camera* pDevCam = &PrimtTech::ComponentHandler::GetComponentByIndex<pt::Camera>(0);


	if (canMove)
	{
		if (KeyboardHandler::IsKeyDown(Key::A))
			move += pDevCam->Getleft();
		if (KeyboardHandler::IsKeyDown(Key::D))
			move += -pDevCam->Getleft();
		if (KeyboardHandler::IsKeyDown(Key::W))
			move += pDevCam->GetForwardV();
		if (KeyboardHandler::IsKeyDown(Key::S))
			move += -pDevCam->GetForwardV();
		if (KeyboardHandler::IsKeyDown(Key::SPACE))
			move += {0.f, 1.f, 0.f};
		if (KeyboardHandler::IsKeyDown(Key::SHIFT))
			move += {0.f, -1.f, 0.f};
		move.Normalize();

		move *= 10.f;

		pDevTransform->Move(move * deltatime);

	}

	while (!MouseHandler::BufferIsEmpty())
	{
		MouseEvent me = MouseHandler::ReadEvent();
		if (me.GetType() == MouseEvent::EventType::RAW_MOVE && canMove)
		{
			sm::Vector2 mouseMove = { (float)me.GetPosition().y, (float)me.GetPosition().x };
			mouseMove *= m_mouseSense;
			pDevTransform->Rotate(mouseMove.x, mouseMove.y, 0.f);
			//pDevCam->UpdateView(*pDevTransform);
		}
		//else if (me.GetType() == MouseEvent::EventType::eSCROLLUP && canMove)
		//	pDevTransform->Offset(0.f, 0.f, -0.5f);
		//else if (me.GetType() == MouseEvent::EventType::eSCROLLDOWN && canMove)
		//	pDevTransform->Offset(0.f, 0.f, 0.5f);
		else if (me.GetType() == MouseEvent::EventType::eLEFTCLICKDOWN)
		{
			//float mouseX = (float)me.GetPosition().x;
			//float mouseY = (float)me.GetPosition().y;

			//float winWidth = m_window.getWinWidth();
			//float winHeight = m_window.getWinHeight();

			//float x = (2.f * mouseX) / winWidth - 1.f;
			//float y = 1.f - (2.f * mouseY) / winHeight;

			//sm::Vector4 clipRay(x, y, -1.f, 1.f);

			//sm::Vector4 eyeRay = XMVector4Transform(clipRay, d::XMMatrixInverse(nullptr, pDevTransform->GetProjM()));

			//eyeRay = sm::Vector4(eyeRay.x, eyeRay.y, 1.f, 0.f);

			//sm::Vector4 worldRay = XMVector4Transform(eyeRay, d::XMMatrixInverse(nullptr, pDevTransform->GetViewM()));

			//sm::Vector3 normRay(worldRay.x, worldRay.y, worldRay.z);

			//normRay.Normalize();

			//mp_dxrenderer->Click(normRay);
		}
		else if (me.GetType() == MouseEvent::EventType::eRIGHTCLICKDOWN)
			m_entlist.console.AddLog("mouse hide");
		else if (me.GetType() == MouseEvent::EventType::eRIGHTCLICKRELEASE)
			m_entlist.console.AddLog("mouse show");
	}

	pDevCam->UpdateView(*pDevTransform);

#ifdef _DEBUG
	if (KeyboardHandler::IsKeyDown(m_optionkey))
		m_entlist.console.AddLog("exit");
#endif // _DEBUG
}

void Editor::Run()
{
	double start = 0, deltaTime = 0;
	start = omp_get_wtime();

	while (m_primtech.IsOpen())
	{
		start = omp_get_wtime();
		m_primtech.Run();
		Update((float)deltaTime);

		while (!m_msgQueue.empty())
		{
			Editor::Messages postMsg = m_msgQueue.front();
			switch (postMsg)
			{
			case Editor::Messages::eQuit:
				m_primtech.Close();
				break;
			case Editor::Messages::eToggleMouse:
				m_primtech.ToggleMouse();
				break;
			case Editor::Messages::eHideMouse:
				m_primtech.HideCursor();
				break;
			case Editor::Messages::eShowMouse:
				m_primtech.ShowCursor();
				break;
			default:
				break;
			}
			m_msgQueue.pop();
		}

		deltaTime = omp_get_wtime() - start;
		m_primtech.SetDeltaTime(deltaTime);
	}
}
