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
	void PhysicsBody::SetType(rp::BodyType bodyType)
	{
		mp_rigidBody->setType(bodyType);
	}
}
