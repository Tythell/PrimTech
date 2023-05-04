#include "pch.h"
#include "Component.h"



namespace pt
{
	Component::Component(const EntIdType id) : m_entId(id)
	{
	}

	const EntIdType Component::EntId() const
	{
		return m_entId;
	}

	void Component::freeComponent(EntIdType idx)
	{
		m_entId = idx;
	}

	

}