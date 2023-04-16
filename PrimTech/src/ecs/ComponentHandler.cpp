#include "pch.h"
#include"ComponentHandler.h"

namespace PrimtTech
{
	COMPONENT_LIST(, ComponentHandler::);

	int ComponentHandler::freeComponents[];
	int ComponentHandler::GetNoFreeComponents(int index)
	{
		return freeComponents[ptm::firstset(index)];
	}
	void ComponentHandler::IncreaseFreeComponents(int index, int n)
	{
		freeComponents[ptm::firstset(index)] += n;
	}
}
