#include "WindowFuncs.h"
//#include "PrimTech.h"

void toggleButton(bool& on, std::string str)
{
	bool clickedButton = false;
	if (!on)
	{
		ImGui::PushID(0);
		ImGui::PushStyleColor(ImGuiCol_Button,			ImVec4(0.2f, 0.2f, 0.2f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered,	ImVec4(0.3f, 0.3f, 0.3f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,	ImVec4(0.4f, 0.4f, 0.4f, 1.f));
	}
	if (ImGui::Button(str.c_str())) clickedButton = true;
	
	if (!on)
	{
		ImGui::PopStyleColor(3);
		ImGui::PopID();
	}
	if (clickedButton) on = !on;
}

enum class Limb
{
	torso, head, lArm, lLeg, rArm, rLeg,
	torso_, head_, lArm_, lLeg_, rArm_, rLeg_
};

void figure(std::string face, uint startIndex, bool* enables, bool show = true)
{
	if (show)
	{
		std::string startIndexStr = std::to_string(startIndex);
		std::string label = "mini##" + startIndexStr;
		ImGui::BeginChild(label.c_str(), ImVec2(90, 80), false, ImGuiWindowFlags_NoScrollbar);
		ImGui::Text("  "); ImGui::SameLine();

		toggleButton(enables[(int)Limb::head + startIndex], face.c_str());
		toggleButton(enables[(int)Limb::rArm + startIndex], " ##rArm" + startIndexStr); ImGui::SameLine();
		toggleButton(enables[(int)Limb::torso + startIndex], "   ##torso" + startIndexStr);  ImGui::SameLine();
		toggleButton(enables[(int)Limb::lArm + startIndex], " ##larm" + startIndexStr);
		ImGui::Text("  "); ImGui::SameLine(); toggleButton(enables[(int)Limb::rLeg + startIndex], " ##rLeg" + startIndexStr); ImGui::SameLine();
		toggleButton(enables[(int)Limb::lLeg + startIndex], " ##lLeg" + startIndexStr);
		ImGui::EndChild();
	}
}

void Gui_MenuBar(void* args, bool* b)
{
	Gui_MenuToggles* arg = (Gui_MenuToggles*)args;
	//ImGui::Begin("imageList");
	//uint numTextures = PrimtTech::ResourceHandler::GetNumTextures();
	//for (uint i = 0; i < numTextures; i++)
	//{
	//	ImTextureID texture = PrimtTech::ResourceHandler::GetTextureAdress(i)->GetSRV();
	//	ImGui::Image(texture, ImVec2(100, 100), { 0.f,1.f }, { 1.f,0.f });
	//}
	//ImGui::End();

	ImGui::BeginMainMenuBar();
	
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Open skin", ""))
		{
			std::string path = Dialogs::OpenFile(".png");

			if (path != "") arg->commands->push("load texture " + path);
		}
		if (ImGui::MenuItem("Reload skin", "CTRL+S"))
		{
			arg->commands->push("reload texture");
		}
		ImGui::EndMenu();
		//ImGui::Separator();
	}
	if (ImGui::BeginMenu("Settings"))
	{
		if (ImGui::MenuItem("Always on top", "", &arg->isAlwaysOnTop))
		{
			std::string command = "setting topmost " + std::to_string((int)arg->isAlwaysOnTop);
			arg->commands->push(command);
		}
		if (ImGui::MenuItem("Hook Keyboard", "", &arg->isHookKeyboard))
		{
			std::string command = "setting kbHook " + std::to_string((int)arg->isHookKeyboard);
			arg->commands->push(command);
		}
		ImGui::Separator();
		ImGui::MenuItem("Toggle Figure", "", arg->toggleFigure);
		ImGui::MenuItem("Toggle Hatfigure", "", arg->toggleFigure + 1);
		ImGui::MenuItem("Toggle Button", "", arg->toggleFigure + 2);
		ImGui::EndMenu();
	}
	static bool hookKb = false;
	static bool alwaysOnTop = false;
	ImGui::BeginDisabled();
	if (ImGui::BeginMenu("Animations"))
	{
		if (ImGui::MenuItem("Walking", "")) {}
		//if (ImGui::MenuItem("Walking", "CTRL+Z")) {}
		ImGui::EndMenu();
	}
	ImGui::EndDisabled();

	ImGui::EndMainMenuBar();
}

void Gui_TestWindow(void* args, bool* b)
{
	ImGui::Begin("TestWindow", b);

	ImGui::Text("Hello world");

	ImGui::End();
}

void Gui_ToggleWindow(void* args, bool* b)
{
	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoTitleBar;
	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoBackground;

	ImGui::Begin("Toggle parts", NULL, flags);

	ImGui::SetWindowSize(ImVec2(150, 270));
	ImGui::SetWindowPos(ImVec2(0, 17));
	
	ToggleWindowStructure* str = (ToggleWindowStructure*)args;


	Gui_MenuToggles* arg = (Gui_MenuToggles*)args;


	/*
	std::vector<std::string> names;

	names.emplace_back("torso");
	names.emplace_back("head");
	names.emplace_back("lArm");
	names.emplace_back("lLeg");
	names.emplace_back("rArm");
	names.emplace_back("rLeg");

	ImGui::BeginChild("left", ImVec2(80, 150), false);
	for (int i = 0; i < names.size(); i++)
	{
		std::string label = names[i];
		ImGui::Checkbox(label.c_str(), &enables[i]); //ImGui::SameLine();
		//label = "##" + label + "hat";
		//ImGui::Checkbox(label.c_str(), &enables[i * 2 + 1]);
	}
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("Right", ImVec2(50, 150), false);
	for (int i = 0; i < names.size(); i++)
	{
		std::string label = "##" + names[i];
		ImGui::Checkbox(label.c_str(), &enables[i + 6]); //ImGui::SameLine();
		//label = "##" + label + "hat";
		//ImGui::Checkbox(label.c_str(), &enables[i * 2 + 1]);
	}
	ImGui::EndChild();*/
	
	std::string faces[] =
	{
		"T_T",
		"UwU",
		"o-o",
		"u-u",
		"o o",
		"| |",
		"-_-",
		". .",
		"O_O",
		"? ?",
		"- -"
	};


	static std::string face = faces[rand() % ARRAYSIZE(faces)] + "##head";

	figure(face, 0, str->enables, str->toggleFigure[0]);
	figure("hat##head", 6, str->enables, str->toggleFigure[1]);

	char textStr[32];

	strcpy_s(textStr, 32, StringHelper::GetName(str->skinFile).c_str());

	if (str->toggleFigure[2] && ImGui::Button(textStr))
	{
		std::string path = Dialogs::OpenFile(".png");

		if (path != "")
			str->commands->push("load texture " + path);

	}

	//ImGui::SameLine();
	//if (ImGui::Button("reload skin"))
		//str->commands->push("reload texture");



	//ImGui::InputText("##skinfilename", textStr, 16, ImGuiInputTextFlags_ReadOnly);

	ImGui::End();



	//ImGui::ShowDemoWindow();
}

/*	if (ImGui::GetIO().MouseClicked[1])
	{
		ImGui::OpenPopup("rclickMenu", ImGuiPopupFlags_MouseButtonRight);
	}

	if (ImGui::BeginPopup("rclickMenu"))
	{
		if (ImGui::MenuItem("Always on top", "", &arg->isAlwaysOnTop))
		{
			std::string command = "setting topmost " + std::to_string((int)arg->isAlwaysOnTop);
			arg->commands->push(command);
		}
		if (ImGui::MenuItem("Hook Keyboard", "", &arg->isHookKeyboard))
		{
			std::string command = "setting kbHook " + std::to_string((int)arg->isHookKeyboard);
			arg->commands->push(command);
		}
		//if (ImGui::Selectable(textStr))
		//{
		//	std::string path = Dialogs::OpenFile(".png");
		//	if (path != "") str->commands->push("load texture " + path);
		//}
if (ImGui::Selectable("reload")) str->commands->push("reload texture");

ImGui::EndPopup();
}*/