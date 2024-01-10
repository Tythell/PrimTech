#pragma once
#include <string>
#include "PrimTech.h"

void Gui_ToggleWindow(void* args, bool* b);

struct Gui_MenuToggles
{
	bool isHookKeyboard = true;
	bool isAlwaysOnTop = false;
	//std::queue<std::string> commands;
};
void Gui_MenuBar(void* args, bool* b);



struct ToggleWindowStructure
{
	bool enables[12];
	std::string skinFile = "hej";
	//PrimtTech::TextureMap* pTexturemap;
	std::queue<std::string> commands;
};