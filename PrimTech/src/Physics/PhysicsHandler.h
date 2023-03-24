#pragma once
#undef min
#undef max
#include<reactphysics3d/reactphysics3d.h>
#include"ecs/Entity.h"

namespace rp = reactphysics3d;

namespace PrimtTech
{
	class PhysicsHandler
	{
	private:
		rp::PhysicsCommon m_physics;
		rp::PhysicsWorld* m_pworld = nullptr;
	public:
		PhysicsHandler();
		rp::RigidBody* CreateRigidBody(const pt::TransformComp& transform, rp::BodyType bodyType = rp::BodyType::DYNAMIC);
		rp::CollisionBody* CreateCollissionBody(const pt::TransformComp& transform);

		void Update(float dt);
	};
}



