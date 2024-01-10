#include"pch.h"
#include "Keyboard.h"

uint KeyboardHandler::m_kbFlags = KeyboardHandler::KeyboardStream::WndProc;
bool KeyboardHandler::m_isKeyDown[256] = { false };
std::queue<KeyboardEvent> KeyboardHandler::m_keyboardBuffer;
HHOOK KeyboardHandler::s_hook = NULL;

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

KeyboardEvent KeyboardHandler::ReadEvent()
{
	if (m_keyboardBuffer.empty()) return KeyboardEvent(KeyboardEvent::EventType::eINVALID, Key::SLEEP);
	KeyboardEvent e = m_keyboardBuffer.front();
	m_keyboardBuffer.pop();
	return e;
}

LRESULT CALLBACK HookCallback(int ncode, WPARAM wParam, LPARAM lParam)
{
	if (ncode >= 0)
	{
		CWPRETSTRUCT* info = reinterpret_cast<CWPRETSTRUCT*>(lParam);

		switch (wParam)
		{
		case WM_KEYDOWN:
		{
			char msg = info->lResult;

			KeyboardHandler::SetKeyState(msg, true);

			break;
		}
		case WM_KEYUP:
		{
			char msg = info->lResult;

			KeyboardHandler::SetKeyState(msg, false);

			break;
		}
		default:
			break;
		}
	}

	return CallNextHookEx(KeyboardHandler::s_hook, ncode, wParam, lParam);
}

void KeyboardHandler::InitKeyboardHook()
{
	s_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0);
	m_kbFlags |= KeyboardStream::GlobalHook;
}

void KeyboardHandler::ReleaseHook()
{
	if (s_hook)
	{
		UnhookWindowsHookEx(s_hook);
		m_kbFlags &= ~KeyboardStream::GlobalHook;
	}
}

KeyboardEvent::KeyboardEvent(EventType e, const unsigned char key):
	type(e), key(key)
{}