#include "WindowFuncs.h"
#include "PrimTech.h"
#include <bitset>

void ToggleWindow(void* args, bool* b)
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar;
	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoBackground;

	ImGui::Begin("Toggle parts", NULL, flags);

	ImGui::SetWindowPos(ImVec2(0, 0));
	
	std::vector<std::string> names;

	names.emplace_back("head");
	names.emplace_back("torso");
	names.emplace_back("lArm");
	names.emplace_back("rArm");
	names.emplace_back("lLeg");
	names.emplace_back("rLeg");

	bool* enables = (bool*)args;

	//ImGui::Checkbox("hej", &enables[0]);
	//ImGui::Checkbox("ho", &enables[1]);

	ImGui::BeginChild("left");
	for (int i = 0; i < names.size(); i++)
	{
		std::string label = names[i];
		ImGui::Checkbox(label.c_str(), &enables[i * 2 + 0]); ImGui::SameLine();
		label = "##" + label + "hat";
		ImGui::Checkbox(label.c_str(), &enables[i * 2 + 1]);
	}
	ImGui::EndChild();
	ImGui::SameLine();
	//ImGui::BeginChild("Right");
	ImGui::Text("Hewllo");
	//ImGui::EndChild();


	ImGui::End();
}
