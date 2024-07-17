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
	uint currentCamera = 1;
	int winWidth = -1, winHeight = -1;
	int m_selectedMaterial = -1;
};

class Game
{
public:
	Game(int2 windowRes, HINSTANCE hInstance);
	~Game();

	void UpdateInput(float deltatime);
	void UpdatePlayer(float deltatime);
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
	std::vector<std::pair<float3, float3>> m_startTransforms;
	void Play(char b);

	std::queue<Messages> m_msgQueue;
	
	EntListStruct m_entlist;
	float m_mouseSense = 0.005f;

	//bool m_playing = false;

	void execCommand(std::string cmd);

	PrimtTech::Renderer* m_renderer = nullptr;

	uint m_activeCamEntIndex = 2;

	pt::PrimTech m_primtech;

	uchar m_keyForward = 'W';
	uchar m_keyBack = 'S';
	uchar m_keyLeft = 'A';
	uchar m_keyRight = 'D';
	uchar m_keyUp = Key::SPACE;
	uchar m_keyDown = Key::LSHIFT;
	uchar m_optionkey = Key::ESCAPE;
};