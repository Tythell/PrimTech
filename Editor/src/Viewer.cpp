#include "Viewer.h"
#include "WindowFuncs.h"

Viewer::Viewer()
{
	int width = 500, height = 500;

	m_engine.Init(L"Skin Viewer", NULL, L"wndclass", width, height);

	InitImguiWindows();

	std::vector<pt::Camera>& m_cams = PrimtTech::ComponentHandler::GetComponentArray<pt::Camera>();
	PrimtTech::ResourceHandler::AddMesh("Assets/models/cube.txt");

	//std::string assetFolder = "Assets/";

	std::vector<pt::Entity>& m_ents = pt::Entity::GetAllEnts();

	PrimtTech::Mesh* pMesh = PrimtTech::ResourceHandler::AddMesh("Assets/models/amcguy.obj");

	//pMesh->SubmeshVisible("headhat", false);

	std::vector<PrimtTech::Mesh>& meshes = PrimtTech::ResourceHandler::GetMeshArrayReference();

	m_avatar = &pt::Entity::Create();

	m_avatar->AddComponent<pt::Light>()->SetType(pt::LightType::eAMBIENT);

	//std::vector<pt::TransformComp>& m_transforms = PrimtTech::ComponentHandler::GetComponentArray<pt::TransformComp>();

	m_mesh = m_avatar->AddComponent<pt::MeshRef>();

	//m_camEnt = &pt::Entity::Create();

	m_pCamEnt = &pt::Entity::Create();



	m_cam = m_pCamEnt->AddComponent<pt::Camera>();

	m_cam->SetPerspective(90, width, height, 0.1f, 100.f);

	m_cam->SetPositionOffset(0.f, 0, -2.5f);

	matrix getView = m_cam->GetViewMatrix();

	pt::Material* pMat = PrimtTech::ResourceHandler::AddMaterial("skin");

	pMat->LoadTexture("mcskin.png", pt::TextureType::eDiffuse);

	m_mesh->SetMesh("amcguy.obj");
	m_mesh->SetMaterial("skin");

	float3 d = pMat->GetDiffuseClr();
}

Viewer::~Viewer()
{
	
}

bool Viewer::Run()
{
	while (m_engine.IsOpen())
	{
		m_engine.Run();
		ControlCam();
		UpdateToggles();

		if (KeyboardHandler::IsKeyDown(Key::ESCAPE))
		{
			m_engine.Close();
		}
	}
	return false;
}

void Viewer::ControlCam()
{
	while (!MouseHandler::BufferIsEmpty())
	{
		MouseEvent me = MouseHandler::ReadEvent();

		bool canMove = MouseHandler::GetIsMouseDown(1);

		if (me.GetType() == MouseEvent::EventType::RAW_MOVE && canMove)
		{
			float2 mouseMove = { (float)me.GetPosition().y, (float)me.GetPosition().x };
			mouseMove *= 0.005f;
			
			//m_cam->RotateOffset(mouseMove.x, mouseMove.y, 0.f);
			
			m_pCamEnt->Rotate(mouseMove.x, mouseMove.y, 0.f);

			//pDevCam->UpdateView(*pDevTransform);
		}

		else if (me.GetType() == MouseEvent::EventType::eSCROLLUP)
			m_cam->MoveOffset(0.f, 0.f, 0.5f);
		else if (me.GetType() == MouseEvent::EventType::eSCROLLDOWN)
			m_cam->MoveOffset(0.f, 0.f, -0.5f);
		/*else if (me.GetType() == MouseEvent::EventType::eLEFTCLICKDOWN)
		{
		}*/
	}
	float3 move(0.f);
	//if (KeyboardHandler::IsKeyDown(Key::W)) move += float3(0.f, 0.f, 1.f);
	//if (KeyboardHandler::IsKeyDown(Key::S)) move += float3(0.f, 0.f, -1.f);
	//if (KeyboardHandler::IsKeyDown(Key::A)) move += float3(-1.f, 0.f, 0.f);
	//if (KeyboardHandler::IsKeyDown(Key::D)) move += float3(1.f, 0.f, 0.f);
	//if (KeyboardHandler::IsKeyDown(Key::SPACE)) move += float3(0.f, 1.f, 0.f);
	//if (KeyboardHandler::IsKeyDown(Key::SHIFT)) move += float3(0.f, -1.f, 0.f);
	move *= 0.001f;
	m_pCamEnt->Move(move);
	m_cam->UpdateView(m_pCamEnt->Transform());
}

void Viewer::InitImguiWindows()
{
	m_engine.CreateImGuiWindow(ToggleWindow, m_enables);
}

void Viewer::UpdateToggles()
{
	PrimtTech::Mesh* pmesh = m_mesh->GetMeshContainerP();
	for (int i = 0; i < 12; i++)
	{
		pmesh->SubmeshVisible(i, m_enables[i]);
	}
}
