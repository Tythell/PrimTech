#pragma once
#undef min
#undef max
#include<reactphysics3d/reactphysics3d.h>
#include"Graphics/Buffer.h"
#include"Graphics/AssetLoading/Vertex.h"
//#include"ecs/Component.h"

namespace rp = reactphysics3d;

namespace PrimtTech
{
	class PhysicsHandler
	{
	public:
		PhysicsHandler();
		~PhysicsHandler();

		void Update(float dt);
		rp::RigidBody* CreateRigidBody(const rp::Transform& transform, rp::BodyType bodyType = rp::BodyType::DYNAMIC);

		rp::BoxShape* CreateBoxShape(const float3& extents);
		rp::SphereShape* CreateSphereShape(float radius);
		rp::CapsuleShape* CreateCapsuleShape(float radius, float height);

		void DestroyPhysicsBody(rp::RigidBody*& pRigidBody);
		uint GetNoPhysBodies() const;
#ifdef PHYSDEBUG
		Buffer<PrimtTech::BBVertex> debugTris;
		void EnableDebugDrawing(ID3D11Device*& pDevice, bool b = true);
		void SetDrawDebug(bool b) { m_isDraw = b; };
		void DrawBuffers(ID3D11DeviceContext*& dc);
#endif // _DEBUG
	private:
#ifdef PHYSDEBUG
		rp::DebugRenderer* md_debugRenderer;
#endif // _DEBUG

		rp::PhysicsCommon m_physics;
		rp::PhysicsWorld* m_pworld = nullptr;

		bool m_isDraw = true;
		bool m_running = true;
		ID3D11Device* device = nullptr;
	};
}