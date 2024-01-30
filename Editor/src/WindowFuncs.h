#pragma once
#include <string>
#include "PrimTech.h"

void Gui_ToggleWindow(void* args, bool* b);
void Gui_TestWindow(void* args, bool* b);

struct Gui_MenuToggles
{
	bool isHookKeyboard = false;
	bool isAlwaysOnTop = false;
	bool* toggleFigure = nullptr;
	std::queue<std::string>* commands = nullptr;
	char* pReloadKey = nullptr;
};
void Gui_MenuBar(void* args, bool* b);



struct ToggleWindowStructure
{
	bool enables[12] = { true };
	bool* toggleFigure = nullptr;
	std::string skinFile = "hej";
	//PrimtTech::TextureMap* pTexturemap;
	std::queue<std::string>* commands = nullptr;
};