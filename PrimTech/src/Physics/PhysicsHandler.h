#pragma once
#undef min
#undef max
#include<reactphysics3d/reactphysics3d.h>
#include"Graphics/Buffer.h"
#include"Graphics/Vertex.h"
//#include"ecs/Component.h"

namespace rp = reactphysics3d;

namespace PrimtTech
{
	class PhysicsHandler
	{
	private:
		rp::PhysicsCommon m_physics;
		rp::PhysicsWorld* m_pworld = nullptr;

#ifdef _DEBUG
		rp::DebugRenderer* md_debugRenderer;
#endif // _DEBUG

		bool m_isDraw = true;

		ID3D11Device* device;
	public:
		PhysicsHandler();
		rp::RigidBody* CreateRigidBody(const rp::Transform& transform, rp::BodyType bodyType = rp::BodyType::DYNAMIC);

		rp::BoxShape* CreateBoxShape(const sm::Vector3& extents)
		{
			return m_physics.createBoxShape(rp::Vector3(extents.x, extents.y, extents.z));
		}

		//uint CreateSphereShape(float r)
		//{
		//	rp::SphereShape* shape = m_physics.createSphereShape(r);


		//	return shape;
		//}
		//rp::CollisionBody* CreateCollissionBody(const pt::TransformComp& transform);

#ifdef _DEBUG
		Buffer<PrimtTech::BBVertex> debugTris;
		void EnableDebugDrawing(ID3D11Device*& pDevice, bool b = true);
		void SetDrawDebug(bool b) { m_isDraw = b; };
		void DrawBuffers(ID3D11DeviceContext*& dc);
#endif // _DEBUG
		void Update(float dt);
	};
}