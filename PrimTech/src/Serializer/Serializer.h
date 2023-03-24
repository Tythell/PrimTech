#pragma once
#include "ecs/ComponentHandler.h"
#include "ecs/Entity.h"

namespace PrimtTech
{
	void Export_asci(std::string path, std::vector<pt::Entity>& entList);
	void Export(std::string path, std::vector<pt::Entity>& entList);
	bool Import(std::string path, std::vector<pt::Entity>& entList);
}