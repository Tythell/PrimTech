#include "Mouse.h"

MouseEvent::MouseEvent(EventType type, const int x, const int y):
	m_xPos(-1), m_yPos(-1), m_type(type)
{}

MouseEvent::EventType MouseEvent::GetType() const
{
	return m_type;
}

int2 MouseEvent::GetPosition() const
{
	return {m_xPos, m_yPos};
}

bool MouseHandler::m_mouseButtonDown[3] = {false, false, false};
std::queue<MouseEvent> MouseHandler::m_mouseBuffer;

void MouseHandler::SetMouseButton(int index, bool b)
{
	if (b && !m_mouseButtonDown[index])
	{
		switch (index)
		{
		case eLEFTCLICK:
		{
			MouseEvent e(MouseEvent::EventType::eLEFTCLICK,-1,-1);
			m_mouseBuffer.push(e);
			break;
		}
		case eRIGHTCLICK:
		{
			MouseEvent e(MouseEvent::EventType::eRIGHTCLICK, -1, -1);
			m_mouseBuffer.push(e);
			break;
		}
		case eMIDCLICK:
		{
			MouseEvent e(MouseEvent::EventType::eMIDCLICK, -1, -1);
			m_mouseBuffer.push(e);
			break;
		}
		default:
			break;
		}
	}

	m_mouseButtonDown[index] = b;
}

const bool MouseHandler::GetIsMouseDown(int mouseButton)
{
	return m_mouseButtonDown[mouseButton];
}

void MouseHandler::AddMouseEvent(const MouseEvent me)
{
	m_mouseBuffer.push(me);
}

MouseEvent MouseHandler::ReadEvent()
{
	MouseEvent e = m_mouseBuffer.front();
	m_mouseBuffer.pop();
	return e;
}

bool MouseHandler::CheckMouseEvent(int index)
{
	if (!m_mouseBuffer.empty() && m_mouseBuffer.front().GetType() == index)
	{
		m_mouseBuffer.pop();
		return true;
	}
	return false;
}
