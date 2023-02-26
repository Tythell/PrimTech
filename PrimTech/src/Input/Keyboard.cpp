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

void KeyboardHandler::AddKeyboardEvent(KeyboardEvent e)
{
	m_keyboardBuffer.push(e);
}

void KeyboardHandler::AddRecordEvent(unsigned char key)
{
	m_recordBuffer.push(key);
}

KeyboardEvent KeyboardHandler::ReadEvent()
{
	if (m_keyboardBuffer.empty()) return KeyboardEvent(KeyboardEvent::EventType::eINVALID, Key::SLEEP);
	KeyboardEvent e = m_keyboardBuffer.front();
	m_keyboardBuffer.pop();
	return e;
}

unsigned char KeyboardHandler::ReadRecordKey(bool popOnUse)
{
	if (m_recordBuffer.empty()) return unsigned char(0);
	unsigned char e = m_recordBuffer.front();
	if(popOnUse)
		m_recordBuffer.pop();

	return e;
}

void KeyboardHandler::SetRecording(const bool& b)
{
	m_isRecording = b;
}

bool KeyboardHandler::IsRecording() const
{
	return m_isRecording;
}

bool KeyboardHandler::IsEmpty() const
{
	return m_recordBuffer.empty();
}

KeyboardEvent::KeyboardEvent(EventType e, const unsigned char key):
	type(e), key(key)
{}