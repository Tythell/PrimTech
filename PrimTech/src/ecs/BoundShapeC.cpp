#include"pch.h"
#include"BoundShapeC.h"
#include "TransformC.h"

namespace pt
{
	AABBComp::AABBComp(EntIdType entId): Component(entId)
	{
		m_aabb.Extents = { .5f,.5f,.5f };
	}
	void AABBComp::Update(pt::TransformComp& transform)
	{
		m_aabb.Center = smm::Vector3(
			transform.GetPosition().x + m_posOffset.x, transform.GetPosition().y + m_posOffset.y, transform.GetPosition().z + m_posOffset.z);
		m_isIntersecting = false;
	}
	void AABBComp::SetExtends(const float3& extends)
	{
		memcpy(&m_aabb.Extents.x, &extends.x, sizeof(float) * 3);
	}
	void AABBComp::SetPositionOffset(const float3& pos)
	{
		memcpy(&m_posOffset.x, &pos.x, sizeof(float) * 3);
	}
	smm::Vector3 AABBComp::GetPositionOffset() const
	{
		return m_posOffset;
	}
	bool AABBComp::Intersects(AABBComp& box)
	{
		return m_aabb.Intersects(box.m_aabb);
	}
	void AABBComp::DuplicateFrom(Component* other)
	{
	}
	void AABBComp::OnFree()
	{
	}
	OBBComp::OBBComp(EntIdType entId) : Component(entId) {}
	void OBBComp::Update(const pt::TransformComp& transform)
	{
		m_obb.Center = { 
			transform.GetPosition().x,
			transform.GetPosition().y,
			transform.GetPosition().z,
		};
		m_obb.Orientation = {
			transform.GetRotationQuaternion().x,
			transform.GetRotationQuaternion().y,
			transform.GetRotationQuaternion().z,
			transform.GetRotationQuaternion().w
		};
	}
	void OBBComp::SetExtends(const float3& extends)
	{
		memcpy(&m_obb.Extents.x, &extends.x, sizeof(float) * 3);
	}
	void OBBComp::DuplicateFrom(Component* other)
	{
	}
	void OBBComp::OnFree()
	{
	}
	BSphereComp::BSphereComp(EntIdType entId) : Component(entId) {}
	void BSphereComp::Update(const pt::TransformComp& transform)
	{
		m_bsphere.Center = { 
			transform.GetPosition().x,
			transform.GetPosition().y,
			transform.GetPosition().z
		};
	}
	void BSphereComp::SetRadius(float r)
	{
		m_bsphere.Radius = r;
	}
	void BSphereComp::DuplicateFrom(Component* other)
	{
	}
	void BSphereComp::OnFree()
	{
	}
}