#include"pch.h"
#include "ImGuiHandler.h"
#include "Material.h"
#include "../Utility/CommonDialogs.h"

namespace rnd3rdPart
{
	void embraceTheDarkness()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		//colors[ImGuiCol_DockingPreview]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		//colors[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(8.00f, 8.00f);
		style.FramePadding = ImVec2(5.00f, 2.00f);
		style.CellPadding = ImVec2(6.00f, 6.00f);
		style.ItemSpacing = ImVec2(6.00f, 6.00f);
		style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style.IndentSpacing = 25;
		style.ScrollbarSize = 15;
		style.GrabMinSize = 10;
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 1;
		style.FrameBorderSize = 1;
		style.TabBorderSize = 1;
		style.WindowRounding = 7;
		style.ChildRounding = 4;
		style.FrameRounding = 3;
		style.PopupRounding = 4;
		style.ScrollbarRounding = 9;
		style.GrabRounding = 3;
		style.LogSliderDeadzone = 4;
		style.TabRounding = 4;
	}
}

namespace PrimtTech
{
	void ImGuiHandler::ImGuiInit(HWND& hwnd, ID3D11Device*& device, ID3D11DeviceContext*& dc)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplDX11_Init(device, dc);
		ImGui_ImplWin32_Init(hwnd);
		ImGui::StyleColorsDark();
	}

	void ImGuiHandler::LoadButton(Material* pMaterial, std::string name, unsigned int e, const unsigned int& i)
	{
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

	void ImGuiHandler::ImGuiShutDown()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	//void ImGuiGradientWindow(ImGuiVars& im)
	//{
	//	ImGui::Begin("Gradient");
	//	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	//	ImVec2 full_gradient_size = ImVec2(255, ImGui::GetFrameHeight());
	//	float halfPoint = full_gradient_size.x / 2;
	//	halfPoint += im.gradientOffset;
	//	{
	//		ImVec2 pbS = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
	//		ImVec2 pbE = ImVec2(pbS.x + halfPoint, full_gradient_size.y + pbS.y);
	//		ImVec2 pwS = ImVec2(pbS.x + halfPoint, ImGui::GetCursorScreenPos().y /* + full_gradient_size.y */);
	//		ImVec2 pwE = ImVec2(ImGui::GetCursorScreenPos().x + full_gradient_size.x, (full_gradient_size.y * 1) + pbS.y);
	//		ImU32 col_a = ImGui::GetColorU32(IM_COL32(0, 0, 0, 255));
	//		ImU32 col_b = ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));
	//
	//		ImVec2 gs = ImVec2(pbE.x - ((full_gradient_size.x / 2.f) - im.gradient[0]), pbS.y);
	//		//ImVec2 gs = ImVec2(pbE.x - ((full_gradient_size.x / 2.f) - im.gradient[0]), pbS.y);
	//		ImVec2 ge = ImVec2(pbE.x + im.gradient[1], pwE.y);
	//
	//		draw_list->AddRectFilled(pbS, pbE, col_a);
	//		draw_list->AddRectFilled(pwS, pwE, col_b);
	//		draw_list->AddRectFilledMultiColor(gs, ge, col_a, col_b, col_b, col_a);
	//		ImGui::InvisibleButton("##gradient1", full_gradient_size);
	//		ImGui::SliderFloat2("grad editor", im.gradient, 1.f, 255.f / 2.f, "%.f");
	//		ImGui::SliderFloat("##gradient offset", &im.gradientOffset, -full_gradient_size.x / 2.f, full_gradient_size.x / 2.f, "%.f");
	//		//ImGui::InputText("##imagename", im.buffer, 16);
	//		ImGui::SameLine();
	//		if (ImGui::Button("Export"))
	//		{
	//			//unsigned char data[255];
	//			//ExportToon(im.buffer, data, im.gradientOffset, im.gradient[0], im.gradient[1]);
	//		}
	//	}
	//
	//	ImGui::End();
	//}

	void ImGuiHandler::ImguiRender()
	{
		for (int i = 0; i < m_windows.size(); i++)
		{
			if (!m_showWin[i] || (*m_showWin[i])) m_windows[i](m_funcVars[i], m_showWin[i]);
		}
	}

	//void ImGuiHandler::ImguiDebug()
	//{
		//ImGui::Begin("Debug", &im.showDebugWindow);
		//ImGui::Text("Press \"TAB\" to toggle mouse");

		//if (ImGui::IsWindowHovered())
		//	im.m_isHoveringWindow = true;


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
		//if (ImGui::CollapsingHeader("General"))
		//{
		//	ImGui::RadioButton("local", (int*)&im.transformMode, 0); ImGui::SameLine();
		//	ImGui::RadioButton("world", (int*)&im.transformMode, 1);
		//	if (ImGui::SliderInt("FOV", &im.fov, 40, 110))
		//		m_pcamHandler->GetCurrentCamera()->SetPerspective((float)im.fov, (float)im.width / (float)im.height, .1f, 100.f);
		//	ImGui::Checkbox("Vsync", &im.useVsync); ImGui::SameLine();
		//	ImGui::Checkbox("Handmodel", &im.enableHandModel); ImGui::SameLine();
		//	ImGui::Checkbox("Raycast", &im.drawRayCast);
		//	ImGui::SameLine();
		//	ImGui::Checkbox("BCircle", &im.drawBCircle);
			//if (im.drawBCircle && ImGui::SliderInt("sphere point count", &im.points, 1, 8))
			//	m_sphere.Init(device, dc, im.points);
		//}

		//if (ImGui::CollapsingHeader("Light"/*, ImGuiTreeNodeFlags_DefaultOpen */))
		//{
		//	ImGui::DragFloat4("Ambient", im.ambient, 0.002f, 0.f, 1.f);
		//	ImGui::DragFloat3("Pointlight Position", im.pointLightPos, 0.01f);
		//	ImGui::DragFloat3("PointLightColor", im.pointLightColor, 0.01f, 0.f, 1.f);
		//	m_bulb.GetMaterial().SetRimColor({ im.pointLightColor[0], im.pointLightColor[1], im.pointLightColor[2] });
		//	ImGui::SliderFloat("PointLight Strength", &im.pointLightStr, 0.f, 3.f);
		//	ImGui::SliderFloat("Specular power", &im.specPow, 1.f, 1000.f);
		//	ImGui::SliderFloat("Distance", &im.pointLightDistance, 0.f, 100.f);

		//	std::string shadowCamString = "Shadow cam dir: " + GetVectorAsString(m_shadowmap.GetShadowCam().GetForwardVector());
		//	ImGui::Text(shadowCamString.c_str());

		//	ImGui::SliderFloat("Attenuation", &im.atten, 0.f, 10.f);
		//}

		//if (ImGui::CollapsingHeader("SpotLight"/*, ImGuiTreeNodeFlags_DefaultOpen */))
		//{
		//	ImGui::DragFloat3("Position##sl", im.sl.position, 0.01f);
		//	if (ImGui::Button("Move to cam##sl"))
		//	{
		//		//m_spotlight.SetPosition(mp_cam->GetPosition());
		//		//m_spotlight.SetRotation(mp_cam->GetForwardVector());
		//		im.sl.position[0] = mp_currentCam->GetPosition().x;
		//		im.sl.position[1] = mp_currentCam->GetPosition().y;
		//		im.sl.position[2] = mp_currentCam->GetPosition().z;

		//		im.sl.rotation[0] = mp_currentCam->GetRotation().x * d::XM_PIDIV2;
		//		im.sl.rotation[1] = mp_currentCam->GetRotation().y * d::XM_PIDIV2;
		//		im.sl.rotation[2] = mp_currentCam->GetRotation().z * d::XM_PIDIV2;
		//	}
		//	//ImGui::DragFloat3("Angle##sl", im.sl.rotation, 0.01f);
		//	//ImGui::DragFloat3("Color##sl", im.pointLightColor, 0.01f, 0.f, 1.f);
		//	//m_bulb.GetMaterial().SetRimColor({ im.pointLightColor[0], im.pointLightColor[1], im.pointLightColor[2] });
		//	ImGui::SliderFloat("Strength##sl", &im.pointLightStr, 0.f, 3.f);
		//	ImGui::SliderFloat("Specular power##sl", &im.specPow, 1.f, 1000.f);
		//	//ImGui::SliderFloat("Distance", &im.pointLightDistance, 0.f, 100.f);

		//	std::string shadowCamString = "Shadow cam dir: " + GetVectorAsString(m_shadowmap.GetShadowCam().GetForwardVector());
		//	ImGui::Text(shadowCamString.c_str());

		//	ImGui::SliderFloat("Attenuation", &im.atten, 0.f, 10.f);
		//}

		//m_pLightBuffer->Data().pointLightDistance = im.pointLightDistance;
		//m_pLightBuffer->Data().specularInstensity = im.specPow;
		//m_pLightBuffer->Data().shadowDir = { 0.f,-1.f,0.f };
		//m_pLightBuffer->Data().atten = im.atten;

		//m_bulb.SetPosition(im.pointLightPos[0], im.pointLightPos[1], im.pointLightPos[2]);

		//ImGui::Text(GetVectorAsString(mp_currentCam->GetRotation()).c_str());
		//ImGui::End();
	//}

	//void ImGuiHandler::CalculateFps(float deltatime)
	//{
	//	static float fpsTimer = 0.f;
	//	//static int fpsCounter = 0;
	//	fpsTimer += deltatime;
	//	//fpsCounter++;
	//	//if (fpsTimer >= 1.f)
	//	//{
	//	//	m_fps = fpsCounter;
	//	//	fpsCounter = 0;
	//	//	fpsTimer = 0.f;
	//	//}
	//	if (fpsTimer >= 1.f)
	//	{
	//		fpsTimer = 0.f;
	//		im.m_fps = (uint)(1.f / deltatime);
	//	}
	//}

	void PrimtTech::ImGuiHandler::AddWindowFunc(ImGuiWindowFunc func, void* pVars, bool* showWin)
	{
		m_windows.emplace_back(func);
		m_funcVars.emplace_back(pVars);
		m_showWin.emplace_back(showWin);
	}

}

