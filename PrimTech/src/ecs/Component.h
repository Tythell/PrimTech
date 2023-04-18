#pragma once
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"
#include "Graphics/CbufferTypes.h"
#include "Physics/PhysicsHandler.h"
#include <lua/lua.hpp>
#include<string>

namespace rp = reactphysics3d;
typedef int EntIdType;

namespace pt
{
	class Component
	{
	public:
		Component(EntIdType id);
		const EntIdType EntId() const;
		void FreeComponent() { m_entId = -1; }
	private:
		EntIdType m_entId;
	};

}