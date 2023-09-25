#include "pch.h"
#include "PhysicsBody.h"
#include "TransformC.h"
#include "Entity.h"

namespace pt
{
	//rp::PhysicsCommon* PhysicsBody::pComonPhys = nullptr;
	//rp::PhysicsWorld* PhysicsBody::pPhysWorld = nullptr;
	PrimtTech::PhysicsHandler* PhysicsBody::m_pPhysHandle = nullptr;

	PhysicsBody::PhysicsBody(EntIdType id) : Component(id)
	{
		int h = PrimtTech::ComponentHandler::GetNoOfUsedComponents<pt::PhysicsBody>();
		Entity::GetEntity(id).SetPhysBodyIndex(h);
	}

	void PhysicsBody::UpdateTransform(pt::TransformComp& transform)
	{
		if (mp_rigidBody)
		{
			const reactphysics3d::Transform& rpTransform = mp_rigidBody->getTransform();

			rp::Vector3 rpPos(rpTransform.getPosition());
			rp::Quaternion rpQuat(rpTransform.getOrientation());
			float3 smPos(rpPos.x, rpPos.y, rpPos.z);
			quat smQuat(rpQuat.x, rpQuat.y, rpQuat.z, rpQuat.w);

			transform.SetPosition(smPos);
			transform.SetRotation(smQuat);
		}
	}

	void PhysicsBody::SetPhysicsTransformation(const pt::TransformComp& transform)
	{
		if (mp_rigidBody)
		{
			float3 smPos = transform.GetPosition();
			rp::Vector3 rpPos = { smPos.x,smPos.y,smPos.z };

			quat fuckingAngle = transform.GetRotationQuaternion();
			rp::Quaternion ruckingAngle = { fuckingAngle.x, fuckingAngle.y, fuckingAngle.z, fuckingAngle.w };
			rp::Transform rptra(rpPos, ruckingAngle);

			mp_rigidBody->setTransform(rptra);
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
	void PhysicsBody::AddBoxColider(const float3& extents)
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
		m_shapeIndexes.erase(m_shapeIndexes.begin() + index);
		mp_rigidBody->removeCollider(mp_rigidBody->getCollider(index));
	}
	void PhysicsBody::SetType(rp::BodyType bodyType)
	{
		//mp_rigidBody->setType(bodyType);
		m_bodyType = bodyType;
	}
	void PhysicsBody::SetPhysicsPosition(const float3& v)
	{
		std::vector<pt::TransformComp>& sss = PrimtTech::ComponentHandler::GetComponentArray<pt::TransformComp>();
		pt::TransformComp& ptTransform = sss[EntId()];
		rp::Transform rpTransform;
		
		rp::BodyType type = mp_rigidBody->getType();
		//Freeze(true);
		rpTransform.setPosition({ v.x, v.y, v.z });

		rp::Vector3 angles(ptTransform.GetRotation().x,
			ptTransform.GetRotation().y,
			ptTransform.GetRotation().z);

		rp::Quaternion rpQ = rpQ.fromEulerAngles(angles);

		//rpTransform.getOpenGLMatrix

		rpTransform.setOrientation(rpQ);

		mp_rigidBody->setTransform(rpTransform);
		mp_rigidBody->resetTorque();
		mp_rigidBody->resetForce();
		//Freeze(false);
	}

	void PhysicsBody::SetPhysicsEulerRotation(const float3& v)
	{
		rp::BodyType type = mp_rigidBody->getType();
		mp_rigidBody->setType(rp::BodyType::STATIC);

		rp::Quaternion q = q.fromEulerAngles(v.x, v.y, v.z);

		pt::TransformComp ptTransform = Entity::GetEntity(EntId()).Transform();
		rp::Transform rpTransform;
		rpTransform.setOrientation(q);
		rpTransform.setPosition({
			ptTransform.GetPosition().x,
			ptTransform.GetPosition().y,
			ptTransform.GetPosition().z});

		mp_rigidBody->setTransform(rpTransform);
		mp_rigidBody->resetTorque();
		mp_rigidBody->resetForce();
		mp_rigidBody->setType(type);
	}

	void PhysicsBody::SetPhysicsQuatRotation(const quat& q)
	{
		pt::TransformComp ptTransform = Entity::GetEntity(EntId()).Transform();
		rp::Transform rpTransform;
		//rpTransform.setToIdentity();
		rp::BodyType type = mp_rigidBody->getType();
		mp_rigidBody->setType(rp::BodyType::STATIC);

		rp::Quaternion rpq(q.x, q.y, q.z, q.w);

		rpTransform.setOrientation(rpq);
		rpTransform.setPosition({
			ptTransform.GetPosition().x,
			ptTransform.GetPosition().y,
			ptTransform.GetPosition().z });

		mp_rigidBody->setTransform(rpTransform);
		mp_rigidBody->resetTorque();
		mp_rigidBody->resetForce();
		mp_rigidBody->setType(type);
	}

	

	void PhysicsBody::Freeze(bool b)
	{
		if (b) mp_rigidBody->setType(rp::BodyType::STATIC);
		else mp_rigidBody->setType(m_bodyType);
	}

	float3 PhysicsBody::GetExtents(uint index) const
	{
		rp::Vector3 ve = dynamic_cast<rp::BoxShape*>(mp_rigidBody->getCollider(index)->getCollisionShape())->getHalfExtents();
		return float3(ve.x, ve.y, ve.z);
	}
	float PhysicsBody::GetSphereRadius(uint index) const
	{
		return dynamic_cast<rp::SphereShape*>(mp_rigidBody->getCollider(index)->getCollisionShape())->getRadius();
	}
	float2 PhysicsBody::GetCapsuleLengths(uint index) const
	{
		rp::CapsuleShape* shape = dynamic_cast<rp::CapsuleShape*>(mp_rigidBody->getCollider(index)->getCollisionShape());
		return float2(shape->getRadius(), shape->getHeight());
	}
	uint PhysicsBody::GetNoColliders() const
	{
		return mp_rigidBody->getNbColliders();
	}

	void PhysicsBody::Delete()
	{
		for (int i = 0; i < GetNoColliders(); i++)
			RemoveCollider(i);
		m_pPhysHandle->DestroyPhysicsBody(mp_rigidBody);
	}

	void PhysicsBody::DuplicateFrom(Component* other)
	{
		PhysicsBody* otherComp = dynamic_cast<PhysicsBody*>(other);
		m_bodyType = otherComp->m_bodyType;
		//m_shapeIndexes = otherComp->m_shapeIndexes;
		if (!mp_rigidBody)
		{
			mp_rigidBody = m_pPhysHandle->CreateRigidBody(
				ptTransformToRp(
					PrimtTech::ComponentHandler::GetComponentByIndex<TransformComp>(other->EntId())));

			Freeze(true);
		}

		for (int i = 0; i < otherComp->m_shapeIndexes.size(); i++)
		{
			switch (otherComp->m_shapeIndexes[i])
			{
			case pt::PhysicsBody::ePT_ShapeType::Box:
				AddBoxColider(otherComp->GetExtents(i));
				break;
			case pt::PhysicsBody::ePT_ShapeType::Sphere:
				AddSphereColider(otherComp->GetSphereRadius(i));
				break;
			case pt::PhysicsBody::ePT_ShapeType::Capsule:
				AddCapsuleColider(otherComp->GetCapsuleLengths(i).x, otherComp->GetCapsuleLengths(i).y);
				break;
			default:
				break;
			}
		}
	}

	rp::Transform PhysicsBody::ptTransformToRp(const pt::TransformComp& t) const
	{
		rp::Transform rpTransform;
		rpTransform.setPosition({ t.GetPosition().x, t.GetPosition().y, t.GetPosition().z });
		rpTransform.setOrientation({
			t.GetRotationQuaternion().x,
			t.GetRotationQuaternion().y,
			t.GetRotationQuaternion().z,
			t.GetRotationQuaternion().w });

		return rpTransform;
	}

	void PhysicsBody::OnFree()
	{
	}

	void PhysicsBody::SetBoxExtents(const float3& extents, uint index)
	{
		dynamic_cast<rp::BoxShape*>(mp_rigidBody->getCollider(index)->getCollisionShape())->
			setHalfExtents({ extents.x, extents.y, extents.z });
	}
	void PhysicsBody::PhysMove(const float3& v)
	{
		mp_rigidBody->applyWorldForceAtCenterOfMass(rp::Vector3(10000 * v.x, 0 * v.y, 10000 * v.z));
		//mp_rigidBody->setLinearVelocity(rp::Vector3(10000 * v.x, 100 * v.y, 10000 * v.z));
		//mp_rigidBody->setLinearLockAxisFactor(rp::Vector3(10000 * v.x, 100 * v.y, 10000 * v.z));
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
