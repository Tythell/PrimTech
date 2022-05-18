#include "Keyboard.h"

//bool KeyboardHandler::m_isKeyDown[256] = { false };
//std::queue<KeyboardEvent> KeyboardHandler::m_keyboardBuffer;

KeyboardHandler::KeyboardHandler():
	m_isKeyDown()
{
}

KeyboardHandler::~KeyboardHandler()
{
	//delete m_isKeyDown;
}

bool KeyboardHandler::IsKeyDown(const unsigned char key)
{
	return m_isKeyDown[key];
}

void KeyboardHandler::SetKeyState(const unsigned char key, bool b)
{
	m_isKeyDown[key] = b;
}

KeyboardEvent::KeyboardEvent(EventType e, const unsigned char key):
	type(e), key(key)
{}