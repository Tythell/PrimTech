#include "pch.h"
#include "Component.h"

namespace pt
{
	Light::Light(uint entId): Component(entId),
		m_dirOffset(0.f,-1.f,0.f, 0.f)
	{
		m_lightData.clr = { 1.f,1.f, 1.f, 1.f };
		m_lightData.pos = { 1.f,1.f, 1.f, 1.f };
		m_lightData.dire = { 0.f,-1.f, 0.f, 0.f };
	}

	void Light::Update(const TransformComp& transform)
	{
		m_lightData.pos.x = transform.GetPosition().x + m_posOffset.x;
		m_lightData.pos.y = transform.GetPosition().y + m_posOffset.y;
		m_lightData.pos.z = transform.GetPosition().z + m_posOffset.z;

		memcpy(&m_lightData.dire.x, &m_dirOffset.x, sizeof(float) * 3);
	}
	void Light::SetType(uchar type)
	{
		m_type = type;
		switch (type)
		{
		case 0:
			// point
			m_lightData.pos.w = 1.f;
			m_lightData.dire.w = 0.f;
			break;
		case 1:
			// Directional
			m_lightData.pos.w = 0.f;
			m_lightData.dire.w = 1.f;
			break;
		case 2:
			// ambient
			m_lightData.pos.w = 0.f;
			m_lightData.dire.w = 0.f;
			break;
		default:
			break;
		}
	}
}