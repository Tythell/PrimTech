#include "Scene.h"
#include <sstream>

namespace ig = ImGui;

std::string AddComp(const uint& entId, std::string compType)
{
	std::string cmd = "comp ";
	cmd += std::to_string(entId) + " ";
	cmd += compType;
	return cmd;
}

void Gui_EntList(void* test)
{
	EntListStruct* p = (EntListStruct*)test;

	ImGui::SetNextWindowSize(ImVec2(450, 400), ImGuiCond_Once);
	ImGui::Begin("Entity list##ecs");

	if (ImGui::Button(" + ##Buton"))
	{
		p->console.AddLog("create ent");
	} ImGui::SameLine();
	if (ImGui::Button("Deselect"))
	{
		p->console.AddLog("select -1");
	}
	ImGui::BeginChild("Lefty", ImVec2(150, 400), true);
	for (int i = 0; i < p->ents.size(); i++)
	{
		ImGui::BeginDisabled(i == 0);
		if (ImGui::Selectable(std::to_string(i).c_str(), p->selected == i))
			p->selected = i;
		ImGui::EndDisabled();
	}
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("Righty", ImVec2(300, 350), false);
	if (p->selected != -1)
	{
		pt::Entity* pEnt = &p->ents[p->selected];
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("cmpList");
		if (ImGui::BeginPopup("cmpList"))
		{
			if (ImGui::Selectable("MeshRef"))
			{
				p->ents[p->selected].AddComponent<pt::MeshRef>()->Init("cube.txt");
			}
			if (ImGui::Selectable("Camera"))
			{
				p->console.AddLog(AddComp(p->selected, "cam").c_str());
			}
			ImGui::EndPopup();
		}
		sm::Vector3 transform = pEnt->Transform().GetPosition();
		ImGui::DragFloat3("Translate", reinterpret_cast<float*>(&transform), 0.02f);
		pEnt->Transform().SetPosition(transform);

		transform = pEnt->Transform().GetRotation();
		ImGui::DragFloat3("Rotate", reinterpret_cast<float*>(&transform), 0.02f);
		pEnt->Transform().SetRotation(transform);

		transform = pEnt->Transform().GetScale();
		ImGui::DragFloat3("Scale", reinterpret_cast<float*>(&transform), 0.02f);
		pEnt->Transform().SetScale(transform);

		if (p->ents[p->selected].HasComponentType(PrimtTech::ec_meshRef))
		{
			if (ImGui::CollapsingHeader("MeshRef", ImGuiTreeNodeFlags_DefaultOpen))
			{
				pt::MeshRef* mr = p->ents[p->selected].GetComponent<pt::MeshRef>();

				char charbuffer[16]{ "" };
				strcpy_s(charbuffer, mr->GetNameOfMesh().c_str());

				if (ImGui::InputText("mesh", charbuffer, 16, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					mr->Init(std::string(charbuffer));
				}
				if (ImGui::InputText("material", charbuffer, 16, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					mr->SetMaterial(std::string(charbuffer));
				}

			}
		}
		if (p->ents[p->selected].HasComponentType(PrimtTech::ec_cam))
		{
			if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
			{
				pt::CameraComp* mr = p->ents[p->selected].GetComponent<pt::CameraComp>();

				sm::Vector3 pos = mr->GetPositionOffset();
				sm::Vector3 rot = mr->GetRotationOffset();

				ImGui::DragFloat3("Translate##cam", reinterpret_cast<float*>(&pos), 0.02f);
				ImGui::DragFloat3("Rotation##cam", reinterpret_cast<float*>(&rot), 0.02f);

				mr->SetPositionOffset(pos);
				mr->SetRotationOffset(rot);

				mr->UpdateView(pEnt->Transform().GetWorld());

				//im->
			}
		}
	}

	ImGui::EndChild();

	ImGui::End();

	// Gizmo
	if (p->selected != -1)
	{
		//if (ImGuizmo::IsOver() || ImGuizmo::IsUsing())
		//	m_isHoveringWindow = true;

		ImGui::SetNextWindowSize(ImVec2((float)p->winWidth, (float)p->winHeight));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs;
		ImGui::Begin("##GizmoWin", 0, flags);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, (float)p->winWidth, (float)p->winHeight);

		pt::CameraComp& pCam = PrimtTech::ComponentHandler::GetComponentByIndex<pt::CameraComp>(p->currentCamera);
		pt::TransformComp& rTr = PrimtTech::ComponentHandler::GetComponentByIndex<pt::TransformComp>(p->selected);

		sm::Matrix camViewM = pCam.GetViewMatrix();
		sm::Matrix camProj = pCam.GetProjMatrix();
		sm::Matrix world = rTr.GetWorld();

		float* model = reinterpret_cast<float*>(&world);
		const float* proj = reinterpret_cast<const float*>(&camProj);
		const float* view = reinterpret_cast<const float*>(&camViewM);

		static ImGuizmo::OPERATION op = ImGuizmo::OPERATION::TRANSLATE;
		

		ImGuizmo::Manipulate(view, proj, op, ImGuizmo::MODE::LOCAL, model);

		if (ImGuizmo::IsUsing())
		{
			sm::Vector3 pos;
			sm::Vector3 scale;
			sm::Quaternion rot;
			world.Decompose(scale, rot, pos);
			//ImGuizmo::DecomposeMatrixToComponents(&world._11, &pos.x, &rot.x, &scale.x);

			rTr.SetScale(scale);
			rTr.SetRotation(rot);
			rTr.SetPosition(pos);
		}

		ImGui::End();
	}
}

void Gui_AssetList(void* func)
{
	ImGui::SetNextWindowSize(ImVec2(450, 400), ImGuiCond_Once);
	if (ImGui::Begin("Loaded Assets"))
	{
		if (ImGui::BeginTabBar("tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Meshes"))
			{
				ImGui::Text("Meshes"); ImGui::SameLine();
				if (ImGui::Button(" + ##addMesh"))
				{

				}
				ImGui::Separator();
				std::vector<PrimtTech::Mesh>& meshArr = PrimtTech::ResourceHandler::GetMeshArrayReference();
				for (int i = 0; i < meshArr.size(); i++)
				{
					std::string name = meshArr[i].GetName();
					ImGui::Text(name.c_str());
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Materials"))
			{
				ImGui::Text("Materials		"); ImGui::SameLine();
				if (ImGui::Button(" + ##addMaterial"))
				{
					
				}
				ImGui::Separator();
				std::vector<PrimtTech::Material>& meshArr = PrimtTech::ResourceHandler::GetMaterialArrayReference();
				for (int i = 0; i < meshArr.size(); i++)
				{
					std::string name = meshArr[i].GetFileName();
					ImGui::Text(name.c_str());
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}


}

void Gui_Console(void* ptr)
{
	DevConsole* pCon = (DevConsole*)ptr;

	ImGui::Begin("Command console");
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	ImGui::BeginChild("hej barn", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
	if (ImGui::Button("Clear"))
	{
		pCon->history.resize(0);
	}

	for (int i = 0; i < pCon->history.size(); i++)
	{
		ImGui::TextWrapped(pCon->history[i].c_str());
	}

	ImGui::EndChild();

	//ig::ShowDemoWindow

	ImGui::Separator();

	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue /*| ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory*/;
	if (ImGui::InputText("##Input", pCon->m_inputBuffer, ARRAYSIZE(pCon->m_inputBuffer), input_text_flags))
	{
		pCon->AddLog(pCon->m_inputBuffer);
		strcpy_s(pCon->m_inputBuffer, "");
	}

	ImGui::End();
}

/*//int ClickFoo(const sm::Ray& ray, ModelList& models)
	//{
	//	float maxDistance = 100.f;
	//	float t = 0.f;
	//	int index = -1;

	//	d::BoundingSphere transformedSphere;
	//	for (int i = 0; i < models.size(); i++)
	//	{
	//		transformedSphere = models[i]->GetBSphere();
	//		sm::Vector3 center = transformedSphere.Center;

	//		float radius = transformedSphere.Radius;

	//		radius *= GetHighestValue(models[i]->GetScale());

	//		sm::Vector3 transformedCenter = d::XMVector3TransformCoord(center, models[i]->GetWorld());

	//		transformedSphere.Center = transformedCenter;
	//		transformedSphere.Radius = radius;

	//		float dummy = 0.f;
	//		if (ray.Intersects(transformedSphere, dummy))
	//		{
	//			sm::Ray localSpaceRay;
	//			sm::Matrix invWorld = models[i]->GetWorldInversed();
	//			localSpaceRay.position = d::XMVector3TransformCoord(ray.position, invWorld);
	//			localSpaceRay.direction = d::XMVector3TransformNormal(ray.direction, invWorld);
	//			localSpaceRay.direction.Normalize();
	//			for (UINT k = 0; k < models[i]->GetMeshP()->GetNofMeshes(); k++)
	//			{
	//				Buffer<Vertex3D>* pVbuffer = &models[i]->GetMeshP()->GetVBuffer();
	//				UINT nOTriangles = pVbuffer->GetBufferSize() / 3;
	//				for (UINT j = 0; j < nOTriangles; j++)
	//				{
	//					sm::Vector3 tri0 = pVbuffer->Data((j * 3) + 0).position;
	//					sm::Vector3 tri1 = pVbuffer->Data((j * 3) + 1).position;
	//					sm::Vector3 tri2 = pVbuffer->Data((j * 3) + 2).position;
	//					if (localSpaceRay.Intersects(tri0, tri1, tri2, t))
	//					{
	//						// Multiply distnace to world space
	//						t *= GetAvarageValue(models[i]->GetScale());
	//						if (t < maxDistance)
	//						{
	//							maxDistance = t;
	//							index = i;
	//						}
	//					}
	//				}
	//			}

	//			//if (t > maxDistance)
	//			//{
	//			//	maxDistance = t;
	//			//	index = i;
	//			//}

	//		}
	//	}
	//	return index;
	//}
	*/
/*//void Click(const sm::Vector3& dir)
//{
	//if (!m_isHoveringWindow)
	//{
	//	sm::Ray ray;
	//	ray.position = mp_currentCam->GetPosition();
	//	ray.direction = dir;
	//	//#ifdef _DEBUG
	//	if (im->drawRayCast)
	//	{
	//		sm::Vector3 endPos = ray.position + dir;
	//		m_rLine.SetLine(ray.position, endPos);
	//	}
	//	//#endif // _DEBUG
	//	int n = ClickFoo(ray, m_models);
	//	m_selected = n;
	//	m_selectedMtrl = -1;
	//}
//}

//void RecursiveRead(Sceneheaders& header, ModelList& v, std::ifstream& reader)
//{
//	reader.read((char*)&header, 4);
//	switch (header)
//	{
//	case Sceneheaders::eMODEL:
//	{
//		ModelStruct ms;
//		reader.read((char*)&ms, sizeof(ModelStruct));
//		if (ms.modelname[0] == '(')
//		{
//			std::string strCopy(ms.modelname);
//			strCopy = strCopy.substr(3);
//			sprintf_s(ms.modelname, strCopy.c_str());
//		}

//		v.emplace_back(new Model);
//		v[v.size() - 1]->Init(std::string(ms.modelname));
//		v[v.size() - 1]->SetPosition(ms.position);
//		v[v.size() - 1]->SetRotation(ms.rotation);
//		v[v.size() - 1]->SetScale(ms.scale);
//		if (std::string(ms.mtrlname) != "")
//			v[v.size() - 1]->GetMaterial().ImportMaterial(std::string(ms.mtrlname));

//		for (UINT i = 1; i < ms.noOfExtraMats + 1 && ms.noOfExtraMats < 100; i++)
//		{
//			reader.read((char*)&ms.mtrlname, 24);
//			if (ms.mtrlname != std::string(""))
//				v[v.size() - 1]->GetMaterial(i).ImportMaterial(std::string(ms.mtrlname));
//		}
//		RecursiveRead(header, v, reader);
//		break;
//	}
//	default:
//		break;
//	}
//}
*/

Scene::Scene(PrimtTech::ImGuiHandler* pGui, d::XMINT2 windowRes) :m_pGui(pGui)
{
	m_pGui->AddWindowFunc(Gui_EntList, &m_entlist);
	//m_pGui->AddWindowFunc(Gui_EntList, &m_console);
	m_pGui->AddWindowFunc(Gui_AssetList, &m_entlist);
	m_pGui->AddWindowFunc(Gui_Console, &m_entlist.console);

	//int selected;

	m_entlist.winWidth = windowRes.x;
	m_entlist.winHeight = windowRes.y;

	m_entlist.ents.emplace_back();
	m_entlist.ents[0].AddComponent<pt::CameraComp>()->SetPerspective(80.f, (float)windowRes.x / (float)windowRes.y, 0.1f, 100.f);

	PrimtTech::ResourceHandler::ReserveMeshMemory(15);

	PrimtTech::ResourceHandler::AddMesh("Assets/models/cube.txt");
	PrimtTech::ResourceHandler::AddMesh("Assets/models/gunter.obj");
	PrimtTech::ResourceHandler::AddMesh("Assets/models/scuffball.obj");
	PrimtTech::ResourceHandler::AddMesh("Assets/models/Slime.fbx");
	PrimtTech::ResourceHandler::AddMaterial("DefaultMaterial");

	//AddEntity();
}

Scene::~Scene()
{
}

void Scene::execCommand(std::string cmd)
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
			m_entlist.ents.emplace_back();
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
	}
	else if (argBuffer == "comp")
	{
		ss >> argBuffer;
		if (StringHelper::IsNumber(argBuffer))
		{
			uint entId = atoi(argBuffer.c_str());
			ss >> argBuffer;
			if (argBuffer == "cam")
			{
				m_entlist.ents[entId].AddComponent<pt::CameraComp>();
			}
			else if (argBuffer == "meshref")
			{
				m_entlist.ents[entId].AddComponent<pt::MeshRef>();
			}

		}
	}
	else if (argBuffer == "select")
	{
		ss >> argBuffer;
		if (StringHelper::IsNumber(argBuffer))
		{
			m_entlist.selected = atoi(argBuffer.c_str());
		}
		else m_entlist.selected = -1;
	}
}

void Scene::Update()
{
	//  console
	if (!m_entlist.console.cmdQ.empty())
	{
		std::string cmd = m_entlist.console.cmdQ.front();
		execCommand(cmd);
		m_entlist.console.cmdQ.pop();
	}
	//if (!m_entlist.commands.empty())
	//{
	//	//switch (m_entlist.commands.front())
	//	//{
	//	//case SceneCommands::eCreateEntity:
	//	//{
	//	//	m_entlist.commands.pop();

	//	//	m_entlist.m_entlist.ents.emplace_back(0);
	//	//}
	//	//default:
	//	//	break;
	//	//}
	//}
}
