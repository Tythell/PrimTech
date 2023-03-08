#pragma once
#include <queue>
#include<vector>
enum MouseButton {eLEFTCLICK, eRIGHTCLICK, eMIDCLICK };

struct int2
{
	int x = 0, y = 0;
};

class MouseEvent
{
public:
	enum EventType
	{
		eINVALID,
		eLEFTCLICKDOWN, eRIGHTCLICKDOWN, eMIDCLICKDOWN,
		eLEFTCLICKRELEASE, eRIGHTCLICKRELEASE, eMIDCLICKRELEASE,
		RAW_MOVE, eSCROLLUP, eSCROLLDOWN
	};
	MouseEvent(EventType type, const int x, const int y);
	EventType GetType() const;
	int2 GetPosition() const;
private:
	EventType m_type;
	int m_xPos, m_yPos;
};

class MouseHandler
{
public:
	static void SetMouseButton(int index, bool b);
	static const bool GetIsMouseDown(int mouseButton);
	static void AddMouseEvent(const MouseEvent me);
	static MouseEvent ReadEvent();
	static bool CheckMouseEvent(int index);
	static void OnMouseRaw(float x, float y);
	static bool BufferIsEmpty();
	static void OnWheelUp(int x, int y);
	static void OnWheelDown(int x, int y);
private:
	static bool m_mouseButtonDown[3];
	static std::queue<MouseEvent> m_mouseBuffer;
};

