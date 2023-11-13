#include "Viewer.h"

Viewer::Viewer()
{
	int width = 1500, height = 900;

	m_engine.Init(L"Skin Viewer", NULL, L"wndclass", width, height);

	std::vector<pt::Camera>& m_cams = PrimtTech::ComponentHandler::GetComponentArray<pt::Camera>();
	PrimtTech::ResourceHandler::AddMesh("Assets/models/cube.txt");

	//std::string assetFolder = "Assets/";

	std::vector<pt::Entity>& m_ents = pt::Entity::GetAllEnts();

	PrimtTech::ResourceHandler::AddMesh("Assets/models/mcguy.obj");
	PrimtTech::ResourceHandler::AddTexture("mcskin.png");

	std::vector<PrimtTech::Mesh>& meshes = PrimtTech::ResourceHandler::GetMeshArrayReference();

	m_avatar = &pt::Entity::Create();

	//std::vector<pt::TransformComp>& m_transforms = PrimtTech::ComponentHandler::GetComponentArray<pt::TransformComp>();


	m_avatar->SetPosition(0.f, 0.f, 0.f);

	m_mesh = m_avatar->AddComponent<pt::MeshRef>();

	//m_camEnt = &pt::Entity::Create();

	m_cam = m_avatar->AddComponent<pt::Camera>();

	m_cam->SetPerspective(90, width, height, 0.1f, 100.f);

	m_cam->SetPositionOffset(0.f, 1.f, 1.f);
	//m_camEnt->SetRotation(2.f, 0.f, 0.f);


	

	matrix getView = m_cam->GetViewMatrix();

	pt::Material* pMat = PrimtTech::ResourceHandler::AddMaterial("skin");

	pMat->LoadTexture("mcskin.png", pt::TextureType::eDiffuse);


	//m_mesh->SetMesh("avatar.obj");
	//m_mesh->SetMaterial("skin");
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
		if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
		{
			float2 mouseMove = { (float)me.GetPosition().y, (float)me.GetPosition().x };
			mouseMove *= 0.005f;
			m_cam->RotateOffset(mouseMove.x, mouseMove.y, 0.f);
			m_cam->UpdateView(m_avatar->Transform());
			//pDevCam->UpdateView(*pDevTransform);
		}
		//else if (me.GetType() == MouseEvent::EventType::eSCROLLUP && canMove)
		//	pDevTransform->Offset(0.f, 0.f, -0.5f);
		//else if (me.GetType() == MouseEvent::EventType::eSCROLLDOWN && canMove)
		//	pDevTransform->Offset(0.f, 0.f, 0.5f);
		else if (me.GetType() == MouseEvent::EventType::eLEFTCLICKDOWN)
		{
		}
	}
}
