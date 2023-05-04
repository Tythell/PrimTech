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
	void ComponentHandler::SetFreeComponents(int index, int n)
	{
		freeComponents[ptm::firstset(index)] = n;
	}
	void ComponentHandler::ZeroFreeArray()
	{
		for (int i = 0; i < NUM_COMPONENT_TYPES; i++)
		{
			freeComponents[i] = 0;
		}
	}
}
