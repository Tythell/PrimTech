#pragma once
#include "../Graphics/AssetLoading/Mesh.h"
#include "../Graphics/AssetLoading/Material.h"
#include "Graphics/CbufferTypes.h"
#include "Physics/PhysicsHandler.h"
#include <lua/lua.hpp>
#include<string>

namespace rp = reactphysics3d;
typedef int EntIdType;
typedef int CompIdType;

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