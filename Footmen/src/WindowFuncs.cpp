#include "WindowFuncs.h"
#include "PrimTech.h"
#include "Scene.h"
#include "Serializer/Serializer.h"

std::string AddCompString(const uint& entId, std::string compType)
{
	std::string cmd = "comp ";
	cmd += std::to_string(entId) + " ";
	cmd += compType;
	return cmd;
}

void LoadButton(PrimtTech::Material* pMaterial, std::string name, unsigned int e, const unsigned int& i)
{
	using namespace PrimtTech;
	bool diffExpept = (e == 0 && !pMaterial->HasTexture(eDiffuse));
	if (!diffExpept)
		name += pMaterial->GetMapName((TextureType)e);

	ImGui::Text(name.c_str());
	if (diffExpept)
	{
		float diff[3]{ pMaterial->GetDiffuseClr().x, pMaterial->GetDiffuseClr().y, pMaterial->GetDiffuseClr().z };
		ImGui::SameLine();
		std::string nem = "##diffusefloat3" + std::to_string(i);
		ImGui::DragFloat3(nem.c_str(), diff, 0.01f, 0.f, 1.f);
		pMaterial->SetDiffuseClr(diff[0], diff[1], diff[2]);
	}
	ImGui::SameLine();
	std::string buttonName = "Load##" + name + std::to_string(i);
	if (ImGui::Button(buttonName.c_str()))
	{
		std::string newMtrlString = Dialogs::OpenFile("Images (*.png, *.jpg)\0*.png;*.jpg;\0", "Assets\\Textures\\");
		if (newMtrlString != "")
		{
			pMaterial->LoadTexture(newMtrlString, (TextureType)e);
		}
	}
	std::string remove = "Remove##";
	remove += name;
	if (pMaterial->HasTexture(e))
	{
		ImGui::SameLine();
		if (ImGui::Button(remove.c_str()))
		{
			pMaterial->RemoveTexture((TextureType)e);
		}
	}
}

void Gui_MaterialProperties(void* ptr, bool* show)
{
	int selectedMAt = *(int*)ptr;
	if (selectedMAt != -1)
	{
		ImGui::Begin("Properties", show);

		PrimtTech::Material* pMaterial = PrimtTech::ResourceHandler::GetMaterialAdress(selectedMAt);
		std::string matName = pMaterial->GetFileName();
		if (ImGui::CollapsingHeader(matName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			char* nameBuffer = new char[16];
			strcpy_s(nameBuffer, 16, matName.c_str());
			if (ImGui::InputText("##MaterialName", nameBuffer, 16, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				pMaterial->SetName(nameBuffer);
			}

			delete[] nameBuffer;

			LoadButton(pMaterial, "Diffuse: ", PrimtTech::eDiffuse, 0);
			LoadButton(pMaterial, "NormalMap: ", PrimtTech::eNormal, 0);
			LoadButton(pMaterial, "DistMap: ", PrimtTech::eDistortion, 0);
			LoadButton(pMaterial, "OpacityMap: ", PrimtTech::eOpacity, 0);

			sm::Vector2 diffuseSpeed(pMaterial->GetDiffuseScrollSpeed());
			sm::Vector2 distortionSpeed(pMaterial->GetDistortionScrollSpeed());
			bool hasDistMap = pMaterial->HasTexture(PrimtTech::eDistortion);
			bool hasDiffuse = pMaterial->HasTexture(PrimtTech::eDiffuse) || pMaterial->HasTexture(PrimtTech::eNormal) || pMaterial->HasTexture(PrimtTech::eOpacity);

			float diffSpeed[2]{ diffuseSpeed.x, diffuseSpeed.y };
			float distSpeed[2]{ distortionSpeed.x, distortionSpeed.y };

			std::string title = "Diffuse Scroll";
			//ImGui::PushItemWidth(m_width * 0.14f);
			if (hasDiffuse)
			{
				ImGui::SliderFloat2(title.c_str(), diffSpeed, -.5f, .5f);
			}


			title = "Distortion Scroll##";
			if (hasDistMap)
				ImGui::SliderFloat2(title.c_str(), distSpeed, -.5f, .5f);

			title = "Reset Scrollspeed##";
			if (ImGui::Button(title.c_str()))
			{
				for (int i = 0; i < 2; i++)
				{
					diffSpeed[i] = 0.f;
					distSpeed[i] = 0.f;
				}
			}
			ImGui::SameLine();
			title = "Reset scrollvalues##";
			if (ImGui::Button(title.c_str()))
				pMaterial->ResetScrollValue();

			pMaterial->SetDiffuseScrollSpeed(diffSpeed[0], diffSpeed[1]);
			pMaterial->SetDistortionScrollSpeed(distSpeed[0], distSpeed[1]);

			if (hasDistMap)
			{
				float distDivider = pMaterial->GetDistortionDivider();
				title = "Dist divider##";
				ImGui::SliderInt(title.c_str(), (int*)&distDivider, 1, 20);
				pMaterial->SetDistortionDivider((float)distDivider);
			}

			float transparancy = pMaterial->GetTransparancy();
			title = "Transparancy##";
			ImGui::SliderFloat(title.c_str(), &transparancy, 0.f, 1.f);
			pMaterial->SetTransparency(transparancy);

			float tiling = pMaterial->GetTextureScale();
			title = "Tiling##";
			ImGui::SliderFloat(title.c_str(), &tiling, 1.f, 10.f);
			pMaterial->SetTextureScale(tiling);
			if (hasDistMap)
			{
				float distScale = pMaterial->GetTextureScaleDist();

				title = "Dist Scale##";
				ImGui::SliderFloat(title.c_str(), &distScale, 1.f, 10.f);
				pMaterial->SetTextureScaleDist(distScale);
			}

			title = "Import##";
			if (ImGui::Button(title.c_str()))
			{
				std::string savePath = "";
				savePath = Dialogs::OpenFile("Material (*.pmtrl)\0*.pmtrl;\0", "Assets\\pmtrl\\");

				if (savePath != "")
				{
					std::string test = StringHelper::GetExtension(savePath);
					if (!(test != "pmtrl" || test != "mtl"))
						savePath += ".pmtrl";
					pMaterial->ImportMaterial(StringHelper::GetName(savePath));
				}
			}
			ImGui::SameLine();
			title = "Export##";
			if (ImGui::Button(title.c_str()))
			{
				std::string savePath = "";
				savePath = Dialogs::SaveFile("Material (*.pmtrl)\0*.pmtrl\0", "Assets\\pmtrl\\");

				if (savePath != "")
				{
					std::string test = StringHelper::GetExtension(savePath);
					if (test != "pmtrl")
						savePath += ".pmtrl";
					pMaterial->ExportMaterial(savePath);
				}
			}

		}

		ImGui::End();
	}

}

void Gui_ImGuiDemo(void* ptr, bool* show)
{
	ImGui::ShowDemoWindow(show);
}

void ImguiDebug(void* ptr, bool* show)
{
	ImGui::Begin("Render Settings", show);

	PrimtTech::ImGuiVars* im = (PrimtTech::ImGuiVars*)ptr;

	if (ImGui::IsWindowHovered())
		im->m_isHoveringWindow = true;

	//std::string fpsString = "FPS: " + std::to_string(im.m_fps);
	//ImGui::Text(fpsString.c_str());

	//ImGui::Checkbox("Show selection", &im.showSelection);


	//if (ImGui::CollapsingHeader("ShadowMap"))
	//{
	//	ImGui::Checkbox("Shadows", &im.shadowMap); ImGui::SameLine();
	//	ImGui::Checkbox("View Shadowcam", &im.viewshadowcam);
	//	ImGui::DragFloat3("Pos", im.shadowcamPos, 0.1f);

	//	im.shadowcamrotation[0] = m_shadowmap.GetShadowCam().GetRotation().x;
	//	im.shadowcamrotation[1] = m_shadowmap.GetShadowCam().GetRotation().y;
	//	im.shadowcamrotation[2] = m_shadowmap.GetShadowCam().GetRotation().z;
	//	im.shadowcamPos[0] = m_shadowmap.GetShadowCam().GetPosition().x;
	//	im.shadowcamPos[1] = m_shadowmap.GetShadowCam().GetPosition().y;
	//	im.shadowcamPos[2] = m_shadowmap.GetShadowCam().GetPosition().z;

	//	ImGui::DragFloat3("Rotate", im.shadowcamrotation, 0.1f);
	//	ImGui::DragFloat("ShadowBias", &im.shadowBias, 0.001f, 0.0f, 0.5f);
	//	m_shadowmap.GetShadowCam().SetRotation(im.shadowcamrotation[0], im.shadowcamrotation[1], im.shadowcamrotation[2]);
	//	m_shadowmap.GetShadowCam().SetPosition(im.shadowcamPos[0], im.shadowcamPos[1], im.shadowcamPos[2]);
	//	if (ImGui::Button("TP cam"))
	//	{
	//		im.shadowcamrotation[0] = mp_currentCam->GetRotation().x;
	//		im.shadowcamrotation[1] = mp_currentCam->GetRotation().y;
	//		im.shadowcamrotation[2] = mp_currentCam->GetRotation().z;
	//		im.shadowcamPos[0] = mp_currentCam->GetPosition().x;
	//		im.shadowcamPos[1] = mp_currentCam->GetPosition().y;
	//		im.shadowcamPos[2] = mp_currentCam->GetPosition().z;
	//	}
	//	if (ImGui::Button("winname"))
	//	{
	//		::SetWindowTextA(*m_pHWND, "balls");
	//	}
	//}
	if (ImGui::CollapsingHeader("General"))
	{
		//ImGui::RadioButton("local", (int*)&im.transformMode, 0); ImGui::SameLine();
		//ImGui::RadioButton("world", (int*)&im.transformMode, 1);


		ImGui::Checkbox("Vsync", &im->useVsync); ImGui::SameLine();
		//ImGui::Checkbox("Handmodel", &im.enableHandModel); ImGui::SameLine();
		//ImGui::Checkbox("Raycast", &im.drawRayCast);
		//ImGui::SameLine();
		//ImGui::Checkbox("BCircle", &im.drawBCircle);
		//if (im.drawBCircle && ImGui::SliderInt("sphere point count", &im.points, 1, 8))
		//	m_sphere.Init(device, dc, im.points);
	}

	ImGui::End();
}

void Gui_menubar(void* ptr, bool* show)
{
	DevConsole* m_console = (DevConsole*)ptr;
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New scene")) { m_console->AddLog("scene new"); }
		if (ImGui::MenuItem("Open scene")) { m_console->AddLog("scene load"); }
		if (ImGui::MenuItem("Export scene")) { m_console->AddLog("scene export"); }
		if (ImGui::MenuItem("Exit editor")) { m_console->AddLog("exit"); }
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGui::MenuItem("Load Lightwarp"))
		{
			std::string path = Dialogs::OpenFile("Images (*.png)\0*.png;*.jpg", "Assets\\Textures\\");
			std::string cmd = "create lwtex " + path;
			if (!path.empty()) m_console->AddLog(cmd.c_str());
		}
		ImTextureID lightwarptex = PrimtTech::ResourceHandler::GetTextureAdress(1)->GetSRV();
		ImGui::Separator();
		ImGui::Image(lightwarptex, { 255.f, 10.f });
		//ImGui::
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Windows"))
	{
		uint numWindows = m_console->m_showWin.size();
		for (int i = 0; i < numWindows; i++)
		{
			ImGui::MenuItem(m_console->m_showWin[i].second.c_str(), NULL, &m_console->m_showWin[i].first);
		}
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}

void Gui_Console(void* ptr, bool* show)
{
	DevConsole* pCon = (DevConsole*)ptr;

	ImGui::Begin("Command console", show);
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	if (ImGui::Button("Clear"))
	{
		pCon->nHistory = 0;
		pCon->history.resize(0);
	} ImGui::SameLine();
	static bool echo = false;
	ImGui::Checkbox("echo", &echo);
	ImGui::BeginChild("hej barn", ImVec2(0, -footer_height_to_reserve), true, ImGuiWindowFlags_HorizontalScrollbar);

	if (pCon->history.size() > 50)
	{
		pCon->history.pop_front();
	}

	for (int i = 0; i < pCon->history.size(); i++)
	{
		switch (pCon->history[i].second)
		{
		case 0:
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 100, 100, 255));
			break;
		case 1:
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
			break;
		default:
			break;
		}
		if (pCon->history[i].second >= (int)!echo)
		{
			ImGui::TextWrapped(pCon->history[i].first.c_str());

		}

		ImGui::PopStyleColor();
	}

	while (pCon->nHistory < pCon->history.size())
	{
		pCon->nHistory++;
		ImGui::SetScrollHereY(0.f);
	}

	ImGui::EndChild();

	//ig::ShowDemoWindow

	ImGui::Separator();

	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue /*| ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory*/;
	if (ImGui::InputText("Input", pCon->m_inputBuffer, ARRAYSIZE(pCon->m_inputBuffer), input_text_flags))
	{
		pCon->AddLog(pCon->m_inputBuffer, DevConsole::white);
		strcpy_s(pCon->m_inputBuffer, "");
	}

	ImGui::End();
}

void Gui_AssetList(void* ptr, bool* show)
{
	EntListStruct* p = (EntListStruct*)ptr;

	ImGui::SetNextWindowSize(ImVec2(450, 400), ImGuiCond_Once);
	if (ImGui::Begin("Loaded Assets", show))
	{
		if (ImGui::BeginTabBar("tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Meshes"))
			{
				std::vector<PrimtTech::Mesh>& arr = PrimtTech::ResourceHandler::GetMeshArrayReference();
				std::string displayText = "Meshes: ";
				displayText += std::to_string(arr.size()) + "/";
				displayText += std::to_string(arr.capacity());
				ImGui::Text(displayText.c_str()); ImGui::SameLine();
				if (ImGui::Button(" + ##addMesh"))
				{
					p->console.AddLog("create mesh");
				}
				ImGui::Separator();
				std::vector<PrimtTech::Mesh>& meshArr = PrimtTech::ResourceHandler::GetMeshArrayReference();
				for (int i = 0; i < meshArr.size(); i++)
				{
					std::string name = meshArr[i].GetName();
					ImGui::Text(name.c_str());
					if (ImGui::IsItemHovered())
					{
						//ImGui::BeginTooltip();

						//ImGui::Text("joe mama");

						//ImGui::EndTooltip();
						ImGui::SetTooltip("am tooltip");
					}
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Materials"))
			{
				std::vector<PrimtTech::Material>& arr = PrimtTech::ResourceHandler::GetMaterialArrayReference();
				std::string displayText = "Materials: ";
				displayText += std::to_string(arr.size()) + "/";
				displayText += std::to_string(arr.capacity());
				ImGui::Text(displayText.c_str()); ImGui::SameLine();
				if (ImGui::Button(" + ##addMaterial"))
				{
					p->console.AddLog("create material");
				} ImGui::SameLine();
				if (ImGui::Button("Deselect"))
				{
					p->m_selectedMaterial = -1;
				}
				ImGui::Separator();

				for (int i = 0; i < arr.size(); i++)
				{
					std::string name = arr[i].GetFileName();
					if (ImGui::Selectable(name.c_str(), p->m_selectedMaterial == i))
						p->m_selectedMaterial = i;
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Textures"))
			{
				std::vector<PrimtTech::TextureMap*>& arr = PrimtTech::ResourceHandler::GetTextureArrayReference();
				std::string displayText = "Textures: ";
				displayText += std::to_string(arr.size()) + "/";
				displayText += std::to_string(arr.capacity());
				ImGui::Text(displayText.c_str());
				ImGui::Separator();
				for (int i = 0; i < arr.size(); i++)
				{
					std::string name = arr[i]->GetName();
					ImGui::Text(name.c_str());

				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

	}
	ImGui::End();
}

void Gui_EntList(void* test, bool* show)
{
	EntListStruct* p = (EntListStruct*)test;

	ImGui::SetNextWindowSize(ImVec2(450, 400), ImGuiCond_Once);
	ImGui::Begin("Entity list##ecs", show);

	if (ImGui::Button(" + ##Buton"))
	{
		p->console.AddLog("create ent");
	} ImGui::SameLine();
	if (ImGui::Button("Deselect"))
	{
		p->console.AddLog("select -1");
	}
	ImGui::SameLine();
	static bool s_imShowDents = false;
	ImGui::Checkbox("Show Dev Ents", &s_imShowDents);
	ImGui::BeginChild("Lefty", ImVec2(150, 400), true);

	uint numEnts = pt::Entity::NumEnts();

	for (uint i = 4 * (int)!s_imShowDents; i < numEnts; i++)
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
		pt::Entity* pEnt = pt::Entity::GetEntityP((uint)p->selected);
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("cmpList");
		if (ImGui::BeginPopup("cmpList"))
		{
			if (ImGui::Selectable("MeshRef"))
			{
				pEnt->AddComponent<pt::MeshRef>()->Init("cube.txt");
			}
			if (ImGui::Selectable("Camera"))
			{
				p->console.AddLog(AddCompString(p->selected, "cam").c_str());
			}
			if (ImGui::Selectable("AABB"))
			{
				p->console.AddLog(AddCompString(p->selected, "aabb").c_str());
			}
			if (ImGui::Selectable("Light"))
			{
				p->console.AddLog(AddCompString(p->selected, "light").c_str());
			}
			if (ImGui::Selectable("RigidBody"))
			{
				p->console.AddLog(AddCompString(p->selected, "rigidbody").c_str());
			}
			ImGui::EndPopup();
		}
		sm::Vector3 transform = pEnt->Transform().GetPosition();
		ImGui::BeginDisabled(p->selected == 0);
		ImGui::DragFloat3("Translate", reinterpret_cast<float*>(&transform), 0.02f);
		pEnt->Transform().SetPosition(transform);

		transform = pEnt->Transform().GetRotation();
		ImGui::DragFloat3("Rotate", reinterpret_cast<float*>(&transform), 0.02f);
		pEnt->Transform().SetRotation(transform);

		transform = pEnt->Transform().GetScale();
		ImGui::DragFloat3("Scale", reinterpret_cast<float*>(&transform), 0.02f);
		pEnt->Transform().SetScale(transform);
		ImGui::EndDisabled();

		if (pEnt->HasComponentType(PrimtTech::ec_meshRef))
		{
			if (ImGui::CollapsingHeader("MeshRef", ImGuiTreeNodeFlags_DefaultOpen))
			{
				pt::MeshRef* mr = pEnt->GetComponent<pt::MeshRef>();
				ImGui::Text(mr->GetNameOfMesh().c_str());
				ImGui::Separator();

				char charbuffer[16]{ "" };
				strcpy_s(charbuffer, mr->GetNameOfMesh().c_str());

				if (ImGui::InputText("mesh", charbuffer, 16, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					mr->Init(std::string(charbuffer));
				}
				uint matSize = (uint)mr->GetMeshContainerP()->GetMtl().size();
				for (int i = 0; i < matSize; i++)
				{
					std::string s = "material " + std::to_string(i);
					strcpy_s(charbuffer, std::to_string(mr->GetMaterialIndex(i)).c_str());
					if (ImGui::InputText(s.c_str(), charbuffer, 16, ImGuiInputTextFlags_EnterReturnsTrue))
					{
						mr->SetMaterial(std::string(charbuffer), i);
					}
				}
			}
		}
		if (pEnt->HasComponentType(PrimtTech::ec_cam))
		{
			if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
			{
				pt::Camera* mr = pEnt->GetComponent<pt::Camera>();

				sm::Vector3 pos = mr->GetPositionOffset();
				sm::Vector3 rot = mr->GetRotationOffset();

				ImGui::DragFloat3("Translate##cam", reinterpret_cast<float*>(&pos), 0.02f);
				ImGui::DragFloat3("Rotation##cam", reinterpret_cast<float*>(&rot), 0.02f);

				mr->SetPositionOffset(pos);
				mr->SetRotationOffset(rot);

				std::string displaytext = "ForwardVec: " + ptm::GetVectorAsString(mr->GetForwardV());
				ImGui::Text(displaytext.c_str());
				displaytext = "LeftVec: " + ptm::GetVectorAsString(mr->Getleft());
				ImGui::Text(displaytext.c_str());
				displaytext = "UpVec: " + ptm::GetVectorAsString(mr->GetUp());
				ImGui::Text(displaytext.c_str());

				const char* items[] = { "Perspective", "Orthographic" };
				int item_current = (int)mr->IsOrthograpgic();


				if (ImGui::Combo("##combo", &item_current, items, IM_ARRAYSIZE(items)))
				{
					switch (item_current)
					{
					case 0:
						mr->SetPerspective(80.f, (float)p->winWidth / (float)p->winHeight, 0.1f, 100.f);
						break;
					case 1:
						mr->SetOrtographic(20.f, 20.f, 0.1f, 10.f);
						break;
					default:
						break;
					}
				}

				mr->UpdateView(pEnt->Transform());
			}
		}
		if (pEnt->HasComponentType(PrimtTech::ec_aabb))
		{
			if (ImGui::CollapsingHeader("Bounding box", ImGuiTreeNodeFlags_DefaultOpen))
			{
				pt::AABBComp* mr = pEnt->GetComponent<pt::AABBComp>();

				sm::Vector3 v = mr->GetBox().Extents;
				ImGui::DragFloat3("size", reinterpret_cast<float*>(&v), 0.02f);
				mr->SetExtends(v);

				v = mr->GetPositionOffset();
				ImGui::DragFloat3("Position Offset", reinterpret_cast<float*>(&v), 0.02f);
				mr->SetPositionOffset(v);

				//ImGui::Text(ptm::GetVectorAsString(mr->GetBox().Center).c_str());
			}
		}
		if (pEnt->HasComponentType(PrimtTech::ec_light))
		{
			if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
			{
				pt::Light* mr = pEnt->GetComponent<pt::Light>();

				sm::Vector4 v = mr->GetPositionOffset();

				ImGui::DragFloat3("Posiiton", reinterpret_cast<float*>(&v), 0.02f);
				mr->SetOffsetPosition(v);

				v = mr->GetLightData().clr;
				ImGui::DragFloat3("##Color", reinterpret_cast<float*>(&v), 0.02f, 0.f, 1.f);

				ImGui::PushItemWidth(ImGui::GetItemRectSize().x / 3);
				ImGui::SameLine(); ImGui::DragFloat("Color", &v.w, 0.02, 0.f, 10.f);
				mr->SetColor(v);
				ImGui::PopItemWidth();
				v = mr->GetDirectionOffset();
				ImGui::DragFloat3("Direction", reinterpret_cast<float*>(&v), 0.02f, -1.f, 1.f);
				mr->SetDirectionOffset(v);

				const char* items[] = { "Point", "Direction", "Ambient" };
				int item_current = (int)mr->GetType();

				if (ImGui::Combo("##lightcombo", &item_current, items, IM_ARRAYSIZE(items)))
					mr->SetType((uchar)item_current);
			}
		}
		if (pEnt->HasComponentType(PrimtTech::ec_rigidBodies))
		{
			if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen))
			{
				pt::PhysicsBody* mr = pEnt->GetComponent<pt::PhysicsBody>();

				const char* items[] = { "select a component", "static", "kinematic", "dynamic"};
				int item_current = (int)mr->GetType();

				if (ImGui::Combo("##lightcombo", &item_current, items, IM_ARRAYSIZE(items)) && item_current != 0)
				{
					// execute command for specific component
					/*
					ent i rigidbody edit static
					ent i rigidbody edit kinematic
					ent i rigidbody edit dynamic
					*/
				}
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

		pt::Camera& pCam = PrimtTech::ComponentHandler::GetComponentByIndex<pt::Camera>(p->currentCamera);
		pt::TransformComp& rTr = PrimtTech::ComponentHandler::GetComponentByIndex<pt::TransformComp>(p->selected);

		sm::Matrix camViewM = pCam.GetViewMatrix();
		sm::Matrix camProj = pCam.GetProjMatrix();
		sm::Matrix world = rTr.GetWorld();

		float* model = reinterpret_cast<float*>(&world);
		const float* proj = reinterpret_cast<const float*>(&camProj);
		const float* view = reinterpret_cast<const float*>(&camViewM);

		static ImGuizmo::OPERATION op = ImGuizmo::OPERATION::TRANSLATE;

		if (KeyboardHandler::IsKeyDown('Q'))
			op = ImGuizmo::OPERATION::BOUNDS;
		if (KeyboardHandler::IsKeyDown('W'))
			op = ImGuizmo::OPERATION::TRANSLATE;
		else if (KeyboardHandler::IsKeyDown('E'))
			op = ImGuizmo::OPERATION::ROTATE;
		else if (KeyboardHandler::IsKeyDown('R'))
			op = ImGuizmo::OPERATION::SCALE;

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