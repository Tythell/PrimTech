#pragma once
#include "Component.h"


namespace pt
{
	class TransformComp;
	class AABBComp : public Component
	{
	public:
		AABBComp(EntIdType entId);

		void Update(pt::TransformComp& transform);
		void SetExtends(const sm::Vector3& extends);
		void SetPositionOffset(const sm::Vector3& pos);
		sm::Vector3 GetPositionOffset() const;

		bool Intersects(AABBComp& box);

		bool IsIntersecting() const { return m_isIntersecting; }
		void SetIsIntersecting(bool b) { m_isIntersecting = b; }

		d::BoundingBox GetBox() const { return m_aabb; };
	private:
		d::BoundingBox m_aabb;
		sm::Vector3 m_posOffset;
		bool m_isIntersecting = false;

		// Inherited via Component
		virtual void DuplicateFrom(Component* other) override;
	};

	class OBBComp : public Component
	{
	public:
		OBBComp(EntIdType entId);

		void Update(const pt::TransformComp& transform);
		void SetExtends(const sm::Vector3& extends);
	private:
		d::BoundingOrientedBox m_obb;

		// Inherited via Component
		virtual void DuplicateFrom(Component* other) override;
	};

	class BSphereComp : public Component
	{
	public:
		BSphereComp(EntIdType entId);

		void Update(const pt::TransformComp& transform);
		void SetRadius(float r);
	private:
		d::BoundingSphere m_bsphere;

		// Inherited via Component
		virtual void DuplicateFrom(Component* other) override;
	};
}