#include "pch.h"
#include "Component.h"



namespace pt
{
	Component::Component(const EntIdType id) : m_entId(id)
	{
		//InitLuaStuff();
	}

	const EntIdType Component::EntId() const
	{
		return m_entId;
	}

	

}