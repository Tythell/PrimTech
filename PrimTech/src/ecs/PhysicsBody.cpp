#include "pch.h"
#include "Component.h"

namespace pt
{
	//rp::PhysicsCommon* PhysicsBody::pComonPhys = nullptr;
	//rp::PhysicsWorld* PhysicsBody::pPhysWorld = nullptr;
	PrimtTech::PhysicsHandler* PhysicsBody::m_pPhysHandle = nullptr;



	PhysicsBody::PhysicsBody(uint id) : Component(id)
	{
	}

	void PhysicsBody::UpdateTransform(pt::TransformComp& transform)
	{
		if (mp_rigidBody)
		{
			const reactphysics3d::Transform& rpTransform = mp_rigidBody->getTransform();

			rp::Vector3 rpPos(rpTransform.getPosition());
			rp::Quaternion rpQuat(rpTransform.getOrientation());
			sm::Vector3 smPos(rpPos.x, rpPos.y, rpPos.z);
			sm::Quaternion smQuat(rpQuat.x, rpQuat.y, rpQuat.z, rpQuat.w);

			transform.SetPosition(smPos);
			transform.SetRotation(smQuat);
		}
	}
	void PhysicsBody::CreateRigidBody(const pt::TransformComp& ptTransform)
	{
		rp::Transform rpTransform;
		rpTransform.setPosition({ ptTransform.GetPosition().x, ptTransform.GetPosition().y, ptTransform.GetPosition().z });
		rpTransform.setOrientation({
			ptTransform.GetRotationQuaternion().x,
			ptTransform.GetRotationQuaternion().y,
			ptTransform.GetRotationQuaternion().z,
			ptTransform.GetRotationQuaternion().w });


		mp_rigidBody = m_pPhysHandle->CreateRigidBody(rpTransform);
		mp_rigidBody->setType(rp::BodyType::STATIC);
		//mp_rigidBody->addCollider()
	}
	void PhysicsBody::AddBoxColider(const sm::Vector3& extents)
	{
		rp::BoxShape* shape = m_pPhysHandle->CreateBoxShape({ extents.x, extents.y, extents.z });
		m_shapeIndexes.emplace_back(ePT_ShapeType::Box);

		mp_rigidBody->addCollider(shape, rp::Transform::identity());
	}
	void PhysicsBody::AddSphereColider(float radius)
	{
		rp::SphereShape* shape = m_pPhysHandle->CreateSphereShape(radius);
		m_shapeIndexes.emplace_back(ePT_ShapeType::Sphere);

		mp_rigidBody->addCollider(shape, rp::Transform::identity());
	}
	void PhysicsBody::AddCapsuleColider(float radius, float height)
	{
		rp::CapsuleShape* shape = m_pPhysHandle->CreateCapsuleShape(radius, height);
		m_shapeIndexes.emplace_back(ePT_ShapeType::Capsule);

		mp_rigidBody->addCollider(shape, rp::Transform::identity());
	}
	void PhysicsBody::RemoveCollider(uint index)
	{
		mp_rigidBody->removeCollider(mp_rigidBody->getCollider(index));
	}
	void PhysicsBody::SetType(rp::BodyType bodyType)
	{
		mp_rigidBody->setType(bodyType);
	}
	void PhysicsBody::SetPhysicsPosition(const sm::Vector3& v)
	{
		rp::Transform rpTransform;
		//rpTransform.setToIdentity();
		rp::BodyType type = mp_rigidBody->getType();
		mp_rigidBody->setType(rp::BodyType::STATIC);
		rpTransform.setPosition({ v.x, v.y, v.z });
		/*rpTransform.setOrientation({
			ptTransform.GetRotationQuaternion().x,
			ptTransform.GetRotationQuaternion().y,
			ptTransform.GetRotationQuaternion().z,
			ptTransform.GetRotationQuaternion().w });*/
		mp_rigidBody->setTransform(rpTransform);
		mp_rigidBody->resetTorque();
		mp_rigidBody->resetForce();
		mp_rigidBody->setType(type);
	}

	sm::Vector3 PhysicsBody::GetExtents(uint index) const
	{
		rp::Vector3 ve = dynamic_cast<rp::BoxShape*>(mp_rigidBody->getCollider(index)->getCollisionShape())->getHalfExtents();
		return sm::Vector3(ve.x, ve.y, ve.z);
	}
	float PhysicsBody::GetSphereRadius(uint index) const
	{
		return dynamic_cast<rp::SphereShape*>(mp_rigidBody->getCollider(index)->getCollisionShape())->getRadius();
	}
	sm::Vector2 PhysicsBody::GetCapsuleLengths(uint index) const
	{
		rp::CapsuleShape* shape = dynamic_cast<rp::CapsuleShape*>(mp_rigidBody->getCollider(index)->getCollisionShape());
		return sm::Vector2(shape->getRadius(), shape->getHeight());
	}
	uint PhysicsBody::GetNoColliders() const
	{
		return mp_rigidBody->getNbColliders();
	}

	void PhysicsBody::SetBoxExtents(const sm::Vector3& extents, uint index)
	{
		dynamic_cast<rp::BoxShape*>(mp_rigidBody->getCollider(index)->getCollisionShape())->
			setHalfExtents({ extents.x, extents.y, extents.z });
	}
	void PhysicsBody::SetSphereRadius(float r, uint index)
	{
		dynamic_cast<rp::SphereShape*>(mp_rigidBody->getCollider(index)->getCollisionShape())->
			setRadius(r);
	}
	void PhysicsBody::SetCapsuleLengths(float r, float h, uint index)
	{
		rp::CapsuleShape* shape = dynamic_cast<rp::CapsuleShape*>(mp_rigidBody->getCollider(index)->getCollisionShape());
		shape->setHeight(h);
		shape->setRadius(r);
	}
}
