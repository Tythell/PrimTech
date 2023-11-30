#pragma once
#include <string>
#include "PrimTech.h"

void ToggleWindow(void* args, bool* b);

struct ToggleWindowStructure
{
	bool enables[12];
	std::string skinFile = "hej";
	PrimtTech::TextureMap* pTexturemap;
	std::queue<std::string> commands;
};