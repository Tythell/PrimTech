#include "Viewer.h"
//#include "WindowFuncs.h"
#include <ctime>

Viewer::Viewer()
	:
	m_msgSize(0), m_bufferSize(0),
	m_initSharedMem(false)
{
	int width = 500, height = 500;

	srand((unsigned int)time(0));

	if (m_initSharedMem)
	{
		m_comlib = new comlib::RingBuffer(L"skinview", 150, comlib::Consumer);
	}

	for (int i = 0; i < 12; i++)
	{
		m_windowStruct.enables[i] = true;
	}

	//memset(m_enables, (int)true, sizeof(bool) * 12);


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

	m_pCamEnt = &pt::Entity::Create();

	m_cam = m_pCamEnt->AddComponent<pt::Camera>();

	m_cam->SetPerspective(90, width, height, 0.1f, 100.f);

	m_cam->SetPositionOffset(0.f, 0, -2.5f);

	matrix getView = m_cam->GetViewMatrix();

	pt::Material* pMat = PrimtTech::ResourceHandler::AddMaterial("skin");

	//pMat->LoadTexture("etho.png", pt::TextureType::eDiffuse);
	pMat->LoadTexture("mcskin.png", pt::TextureType::eDiffuse);

	m_mesh->SetMesh("amcguy.obj");
	m_mesh->SetMaterial("skin");

	float3 d = pMat->GetDiffuseClr();
}

Viewer::~Viewer()
{
	if (m_comlib)
	{
		delete m_comlib;
		m_comlib = nullptr;
	}
}

bool Viewer::Run()
{
	while (m_engine.IsOpen())
	{
		m_engine.Run();
		ControlCam();
		UpdateToggles();
		UpdateTexture();

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
			
			m_pCamEnt->Rotate(mouseMove.x, mouseMove.y, 0.f);
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
	move *= 0.001f;
	m_pCamEnt->Move(move);
	m_cam->UpdateView(m_pCamEnt->Transform());
}

void Viewer::InitImguiWindows()
{
	m_engine.CreateImGuiWindow(ToggleWindow, &m_windowStruct);
}

void Viewer::UpdateToggles()
{
	PrimtTech::Mesh* pmesh = m_mesh->GetMeshContainerP();
	for (int i = 0; i < 12; i++)
		pmesh->SubmeshVisible(i, m_windowStruct.enables[i]);
}

void Viewer::UpdateTexture()
{
	pt::Material* pMat = PrimtTech::ResourceHandler::GetMaterialAdress(m_mesh->GetMaterialIndex(0));
	PrimtTech::TextureMap* pTexture = pMat->GetTexture(0);

	if (m_initSharedMem)
	{
		bool updateRecieved = ComlibUpdate(pTexture);
		pTexture->Update();
	}
}

bool Viewer::ComlibUpdate(PrimtTech::TextureMap*& pTexture)
{
	char* message = new char[m_msgSize];
	SectionHeader* psh = new SectionHeader;

	bool result = true;

	while (m_comlib->Recieve(message, psh))
	{
		switch (psh->header)
		{
		case Headers::PixelArrayStart:
		{
			PixelArrayStart numPixels = 0;

			memcpy(&numPixels, message, psh->msgLen);

			PixelChange pix{ uint2(0,0), float4(0.f,0.f,0.f,0.f) };
			for (size_t i = 0; i < numPixels; i++)
			{
				m_comlib->Recieve(message, psh);
				memcpy(&pix, message, psh->msgLen);
				pTexture->SetPixelColor(pix.pixel, pix.color);
			}

			break;
		}
		case Headers::LonePixel:
		{
			PixelChange pc = {};
			memcpy(&pc, message, psh->msgLen);
			pTexture->SetPixelColor(pc.pixel, pc.color);

			break;
		}
		default:
			result = false;
			break;
		}
	};

	

	delete psh;
	delete[] message;

	return result;
}

void Viewer::UpdateCommands()
{
	while (!m_windowStruct.commands.empty())
	{
		StringHelper
		if (m_windowStruct.commands.front() == "tex load")
		{

		};
		m_windowStruct.commands.pop();
	}
}
