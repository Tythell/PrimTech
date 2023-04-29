#pragma once
#include "Component.h"

namespace pt
{
	class TransformComp;

	class PhysicsBody : public Component
	{
	public:
		PhysicsBody(EntIdType id);

		enum class ePT_ShapeType
		{
			Box, Sphere, Capsule
		};

		//void AddColider(rp::, const pt::TransformComp& transform);
		void UpdateTransform(pt::TransformComp& transform);

		void CreateRigidBody(const pt::TransformComp& ptTransform);

		void AddBoxColider(const sm::Vector3& extents = { .5f, .5f, .5f });
		void AddSphereColider(float radius);
		void AddCapsuleColider(float radius, float height);
		void RemoveCollider(uint index = 0);

		rp::BodyType GetType() const { return mp_rigidBody->getType(); }
		void SetType(rp::BodyType bodyType);
		void SetPhysicsPosition(const sm::Vector3& v);
		void SetPhysicsEulerRotation(const sm::Vector3& v);
		void SetBoxExtents(const sm::Vector3& extents, uint index = 0);
		void SetSphereRadius(float r, uint index = 0);
		void SetCapsuleLengths(float r, float h, uint index = 0);
		void SetIsTrigger(bool trigger, uint i) { mp_rigidBody->getCollider(i)->setIsTrigger(trigger); }
		bool GetIsTrigger(uint i) const { return mp_rigidBody->getCollider(i)->getIsTrigger(); }

		ePT_ShapeType GetColliderType(uint index = 0) const { return m_shapeIndexes[index]; }

		sm::Vector3 GetExtents(uint index = 0) const;
		float GetSphereRadius(uint index = 0) const;
		sm::Vector2 GetCapsuleLengths(uint index = 0) const; // x = r, y = h
		uint GetNoColliders() const;
		bool Exists() const
		{
			return (mp_rigidBody);
		}

		//void ResetPosition()
		//{
		//	rp::Transform rpTransform;
		//	rpTransform.setPosition({ m_startPos.x,m_startPos.y,m_startPos.z });
		//	rpTransform.setOrientation({
		//		m_startOrientation.x,
		//		m_startOrientation.y,
		//		m_startOrientation.z,
		//		m_startOrientation.w });

		//	mp_rigidBody->setTransform(rpTransform);
		//}
		static void SetPtrs(PrimtTech::PhysicsHandler* pPhysHandle) { m_pPhysHandle = pPhysHandle; }
	private:
		reactphysics3d::RigidBody* mp_rigidBody = nullptr;
		static PrimtTech::PhysicsHandler* m_pPhysHandle;
		std::vector<ePT_ShapeType> m_shapeIndexes;
		sm::Quaternion m_startOrientation;
	};
}