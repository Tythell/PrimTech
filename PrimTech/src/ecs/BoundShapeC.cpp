#include"pch.h"
#include"Component.h"

namespace pt
{
	AABBComp::AABBComp(uint entId): Component(entId)
	{
		m_aabb.Extents = { .5f,.5f,.5f };
	}
	void AABBComp::Update(pt::TransformComp& transform)
	{
		m_aabb.Center = transform.GetPosition() + m_posOffset;
		m_isIntersecting = false;
	}
	void AABBComp::SetExtends(const sm::Vector3& extends)
	{
		m_aabb.Extents = extends;
	}
	void AABBComp::SetPositionOffset(const sm::Vector3& pos)
	{
		m_posOffset = pos;
	}
	sm::Vector3 AABBComp::GetPositionOffset() const
	{
		return m_posOffset;
	}
	bool AABBComp::Intersects(AABBComp& box)
	{
		return m_aabb.Intersects(box.m_aabb);
	}
	void AABBComp::CreateFromMesh(const pt::MeshRef& mr)
	{
		//mr.GetMeshContainerP()
	}
	OBBComp::OBBComp(uint entId) : Component(entId) {}
	void OBBComp::Update(const pt::TransformComp& transform)
	{
		m_obb.Center = transform.GetPosition();
		m_obb.Orientation = transform.GetRotationQuaternion();
	}
	void OBBComp::SetExtends(const sm::Vector3& extends)
	{
		m_obb.Extents = extends;
	}
	BSphereComp::BSphereComp(uint entId) : Component(entId) {}
	void BSphereComp::Update(const pt::TransformComp& transform)
	{
		m_bsphere.Center = transform.GetPosition();
	}
	void BSphereComp::SetRadius(float r)
	{
		m_bsphere.Radius = r;
	}
}