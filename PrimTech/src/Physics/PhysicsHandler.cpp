#include "pch.h"
#include "PhysicsHandler.h"
#include "Macros/Colors.h"

namespace PrimtTech
{
	sm::Vector3 rpToSm(const rp::Vector3& f)
	{
		return sm::Vector3(f.x, f.y, f.z);
	}

	PhysicsHandler::PhysicsHandler()
	{
		rp::PhysicsWorld::WorldSettings rpSettings;

		m_pworld = m_physics.createPhysicsWorld(rpSettings);

	}

	PhysicsHandler::~PhysicsHandler()
	{
#ifdef PHYSDEBUG
		debugTris.Release();
#endif // PHYSDEBUG
	}

	void PhysicsHandler::Update(float dt, bool update)
	{
		if (m_running && m_pworld->getNbRigidBodies() != 0)
			m_pworld->update(dt);

#ifdef PHYSDEBUG
		if (m_isDraw)
		{
			std::vector<PrimtTech::BBVertex> tris;
			std::vector<PrimtTech::BBVertex> lines;

			tris.resize(md_debugRenderer->getNbTriangles() * 3);
			lines.resize(md_debugRenderer->getNbLines() * 2);

			for (int i = 0; md_debugRenderer->getIsDebugItemDisplayed(rp::DebugRenderer::DebugItem::COLLISION_SHAPE) && i < tris.size(); i += 3)
			{
				pt::Color clr = md_debugRenderer->getTrianglesArray()[i / 3].color1;
				tris[i + 0].m_position = rpToSm(md_debugRenderer->getTrianglesArray()[i / 3].point1);
				tris[i + 0].m_color = clr.GetAsVector3();

				sm::Vector4 vec = clr.GetAsVector4();

				clr = md_debugRenderer->getTrianglesArray()[i / 3].color2;
				tris[i + 1].m_position = rpToSm(md_debugRenderer->getTrianglesArray()[i / 3].point2);
				tris[i + 1].m_color = clr.GetAsVector3();

				clr = md_debugRenderer->getTrianglesArray()[i / 3].color3;
				tris[i + 2].m_position = rpToSm(md_debugRenderer->getTrianglesArray()[i / 3].point3);
				tris[i + 2].m_color = clr.GetAsVector3();
			}

			if (tris.size()) debugTris.CreateVertexBuffer(device, tris.data(), tris.size(), NULL, eBufferFlags_IgnoreCreateTwice);
			else
			{
				// To prevent error on exiting without loading vertexbuffer
				PrimtTech::BBVertex vert;
				debugTris.CreateVertexBuffer(device, &vert, 1, NULL, eBufferFlags_IgnoreCreateTwice);
			}
		}
#endif // _DEBUG
		m_running = update;

	}

	rp::BoxShape* PhysicsHandler::CreateBoxShape(const sm::Vector3& extents)
	{
		return m_physics.createBoxShape(rp::Vector3(extents.x, extents.y, extents.z));
	}

	rp::SphereShape* PhysicsHandler::CreateSphereShape(float radius)
	{
		return m_physics.createSphereShape(radius);
	}

	rp::CapsuleShape* PhysicsHandler::CreateCapsuleShape(float radius, float height)
	{
		return m_physics.createCapsuleShape(radius, height);
	}

	rp::RigidBody* PhysicsHandler::CreateRigidBody(const rp::Transform& transform, rp::BodyType bodyType)
	{
		rp::RigidBody* pRigidBody = nullptr;
		//rp::SphereShape* pSphereShape = nullptr;


		pRigidBody = m_pworld->createRigidBody(transform);
		//pSphereShape = m_physics.createSphereShape(1.f);

		//pRigidBody->addCollider(CreateBoxShape({.5f,.5f,.5f}), rp::Transform::identity());

		pRigidBody->setType(bodyType);

		return pRigidBody;
	}

#ifdef PHYSDEBUG
	void PhysicsHandler::EnableDebugDrawing(ID3D11Device*& pDevice, bool b)
	{
		m_isDraw = b;
		device = pDevice;

		m_pworld->setIsDebugRenderingEnabled(m_isDraw);
		md_debugRenderer = &m_pworld->getDebugRenderer();

		md_debugRenderer->setIsDebugItemDisplayed(rp::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
		//md_debugRenderer->setIsDebugItemDisplayed(rp::DebugRenderer::DebugItem::, true);
	}

	void PhysicsHandler::DrawBuffers(ID3D11DeviceContext*& dc)
	{
		uint offset = 0;
		//dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dc->IASetVertexBuffers(0, 1, debugTris.GetReference(), debugTris.GetStrideP(), &offset);
		dc->Draw(debugTris.GetBufferSize(), 0);
	}
#endif // _DEBUG


}