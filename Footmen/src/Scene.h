#pragma once
#include "PrimTech.h"
#include<queue>

enum class SceneCommands
{
	eNull,
	eCreateEntity,

};

struct EntList
{
	std::queue<SceneCommands> commands;
	std::vector<pt::Entity> m_ents;
	int selected = -1;
};

class Scene
{
public:
	Scene(PrimtTech::ImGuiHandler* pGui);
	~Scene();

	//void Update

	void Update();
private:
	PrimtTech::ImGuiHandler* m_pGui = nullptr;
	EntList m_vars;
	//GuiVars* pNu = nullptr;
};