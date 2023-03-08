#pragma once
#include "PrimTech.h"
#include<queue>

struct DevConsole
{
public:
	std::vector<std::string> history;
	std::queue<std::string> cmdQ;
	char m_inputBuffer[256]{""};
	void AddLog(const char* cmd)
	{
		char buf[512]{""};
		strcpy_s(buf, cmd);
		history.push_back(buf);
		cmdQ.push(buf);
	}
};

struct EntListStruct
{
	DevConsole console;
	std::vector<pt::Entity> ents;
	int selected = -1;
	uint currentCamera;
	int winWidth, winHeight;
};

class Scene
{
public:
	Scene(PrimtTech::ImGuiHandler* pGui, d::XMINT2 windowRes);
	~Scene();

	//void Update

	void Update();
private:
	PrimtTech::ImGuiHandler* m_pGui = nullptr;
	EntListStruct m_entlist;

	void execCommand(std::string cmd);
};