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
	int m_selectedMaterial = -1;
};

using uchar = unsigned char;

class Editor
{
public:
	Editor(PrimtTech::ImGuiHandler* pGui, d::XMINT2 windowRes);
	~Editor();

	//void Update

	void Update(float deltatime);
private:
	PrimtTech::ImGuiHandler* m_pGui = nullptr;
	EntListStruct m_entlist;

	

	void execCommand(std::string cmd);

	uchar m_keyForward = 'W';
	uchar m_keyBack = 'S';
	uchar m_keyLeft = 'A';
	uchar m_keyRight = 'D';
	uchar m_keyUp = Key::SPACE;
	uchar m_keyDown = Key::LSHIFT;
};