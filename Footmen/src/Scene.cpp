#include "Scene.h"

void Gui_EntList(void* test)
{
	EntList* im = (EntList*)test;
	ImGui::SetNextWindowSize(ImVec2(450, 400), ImGuiCond_Once);
	ImGui::Begin("Entity list##ecs");

	if (ImGui::Button(" + ##Buton"))
	{
		im->commands.push(SceneCommands::eCreateEntity);
	} ImGui::SameLine();
	if (ImGui::Button("Deselect"))
	{
		im->selected = -1;
	}
	ImGui::BeginChild("Lefty", ImVec2(150, 400), true);
	for (int i = 0; i < im->m_ents.size(); i++)
	{
		if (ImGui::Selectable(std::to_string(i).c_str(), im->selected == i))
			im->selected = i;
	}
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("Righty", ImVec2(300, 350), false);
	if (im->selected != -1)
	{
		pt::Entity* pEnt = &im->m_ents[im->selected];
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("cmpList");
		if (ImGui::BeginPopup("cmpList"))
		{
			if (ImGui::Selectable("MeshRef"))
			{
				im->m_ents[im->selected].AddComponent<pt::MeshRef>()->Init("cube.txt");
			}
			ImGui::EndPopup();
		}
		sm::Vector3 pos = pEnt->Transform().GetPosition();

		ImGui::DragFloat3("Translate", reinterpret_cast<float*>(&pos), 0.02f);
		pEnt->Transform().SetPosition(pos);
		if (im->m_ents[im->selected].HasComponentType(PrimtTech::ec_meshRef))
		{
			if (ImGui::CollapsingHeader("MeshRef", ImGuiTreeNodeFlags_DefaultOpen))
			{
				pt::MeshRef* mr = im->m_ents[im->selected].GetComponent<pt::MeshRef>();

				ImGui::Text("Has meshref");
				
			}
			
		}
	}
	
	ImGui::EndChild();

	ImGui::End();
}

Scene::Scene(PrimtTech::ImGuiHandler* pGui):m_pGui(pGui)
{
	m_vars.m_ents.reserve(5);

	m_pGui->AddWindowFunc(Gui_EntList, &m_vars);

	//AddEntity();
}

Scene::~Scene()
{
}

void Scene::Update()
{
	if (!m_vars.commands.empty())
	{
		switch (m_vars.commands.front())
		{
		case SceneCommands::eCreateEntity:
		{
			m_vars.commands.pop();

			m_vars.m_ents.emplace_back(0);
		}
		default:
			break;
		}
	}
}
