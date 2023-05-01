#pragma once
#include "PrimTech.h"
#include<queue>
#include <bitset>

struct DevConsole
{
	enum MsgType
	{
		gray, white,
	};
public:
	
	std::deque<std::pair<std::string, MsgType>> history;
	std::queue<std::string> cmdQ;
	char m_inputBuffer[256]{""};
	uint nHistory = 0;
	void AddLog(const char* cmd, MsgType type = MsgType(0))
	{
		char buf[512]{""};
		strcpy_s(buf, cmd);
		history.push_back({ buf, type });
		cmdQ.push(buf);
	}
	std::vector<std::pair<bool, std::string>> m_showWin;
};

struct EntListStruct
{
	DevConsole console;
	//std::vector<pt::Entity> ents;
	int selected = -1;
	uint currentCamera = 0;
	int winWidth = -1, winHeight = -1;
	int m_selectedMaterial = -1;
};

class Editor
{
public:
	Editor(d::XMINT2 windowRes, HINSTANCE hInstance);
	~Editor();

	void Update(float deltatime);
	void Run();

	//bool GetIsExit() const { return m_exit; };

	enum class Messages
	{
		eNull,
		eQuit,
		eHideMouse,
		eShowMouse,
		eToggleMouse,
	};
private:
	std::queue<Messages> m_msgQueue;
	PrimtTech::ImGuiHandler m_pGui;
	EntListStruct m_entlist;
	float m_mouseSense = 0.005f;

	//bool m_playing = false;

	void execCommand(std::string cmd);

	PrimtTech::DX11Renderer* m_renderer = nullptr;

	pt::PrimTech m_primtech;

	std::vector<std::pair<sm::Vector3, sm::Vector3>> m_startTransforms;
	void Play(char b = 2);

	//bool m_exit = false;

	uchar m_keyForward = 'W';
	uchar m_keyBack = 'S';
	uchar m_keyLeft = 'A';
	uchar m_keyRight = 'D';
	uchar m_keyUp = Key::SPACE;
	uchar m_keyDown = Key::LSHIFT;
	uchar m_optionkey = Key::ESCAPE;
};