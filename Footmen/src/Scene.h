#pragma once
#include "PrimTech.h"
#include<queue>

struct DevConsole
{
	enum MsgType
	{
		gray, white,
	};
public:
	
	std::vector<std::pair<std::string, MsgType>> history;
	std::queue<std::string> cmdQ;
	char m_inputBuffer[256]{""};
	void AddLog(const char* cmd, MsgType type = MsgType(0))
	{
		char buf[512]{""};
		strcpy_s(buf, cmd);
		history.push_back({ buf, type });
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

	void Update(float deltatime);

	bool GetIsExit() const { return m_exit; };

	enum class Messages
	{
		eNull,
		eQuit,
		eHideMouse,
		eShowMouse,
		eToggleMouse,
	};
	std::queue<Messages> m_msgQueue;
private:
	PrimtTech::ImGuiHandler* m_pGui = nullptr;
	EntListStruct m_entlist;

	void execCommand(std::string cmd);

	bool m_exit = false;

	uchar m_keyForward = 'W';
	uchar m_keyBack = 'S';
	uchar m_keyLeft = 'A';
	uchar m_keyRight = 'D';
	uchar m_keyUp = Key::SPACE;
	uchar m_keyDown = Key::LSHIFT;
	uchar m_optionkey = Key::ESCAPE;
};