#include "pch.h"
#include "PhysicsHandler.h"

namespace PrimtTech
{
	PhysicsHandler::PhysicsHandler()
	{
		rp::PhysicsWorld::WorldSettings rpSettings;

		m_pworld = m_physics.createPhysicsWorld(rpSettings);
	}

	rp::RigidBody* PhysicsHandler::CreateRigidBody(const pt::TransformComp& transform, rp::BodyType bodyType)
	{
		rp::RigidBody* pRigidBody = nullptr;
		rp::SphereShape* pSphereShape = nullptr;


		rp::Transform rpTr;
		rp::Vector3 rpVec(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z);
		rpTr.setPosition(rpVec);
		sm::Quaternion quat(transform.GetRotationQuaternion());
		rp::Quaternion rpQuat(quat.x, quat.y, quat.z, quat.w);
		rpTr.setOrientation(rpQuat);


		pRigidBody = m_pworld->createRigidBody(rpTr);
		pSphereShape = m_physics.createSphereShape(1.f);

		pRigidBody->addCollider(pSphereShape, rp::Transform::identity());

		pRigidBody->setType(bodyType);

		return pRigidBody;
	}

	rp::CollisionBody* PhysicsHandler::CreateCollissionBody(const pt::TransformComp& transform)
	{
		rp::CollisionBody* pCollisionBody = nullptr;


		rp::Transform rpTr;
		rp::Vector3 rpVec(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z);
		rpTr.setPosition(rpVec);
		sm::Quaternion quat(transform.GetRotationQuaternion());
		rp::Quaternion rpQuat(quat.x, quat.y, quat.z, quat.w);
		rpTr.setOrientation(rpQuat);

		//rp::CollisionShapeType::

			//pCollisionBody->addCollider(pCollisionBody, rpTr);

		return pCollisionBody;
	}

	void PhysicsHandler::Update(float dt)
	{
		if(m_pworld->getNbRigidBodies() != 0)
			m_pworld->update(dt);
	}


}


