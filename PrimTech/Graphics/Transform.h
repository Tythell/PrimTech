#pragma once
#include"../Math/Simplemath.h"
namespace sm = DirectX::SimpleMath;
namespace d = DirectX;
class Transform
{
	sm::Vector3 pos, rot, scale;
	sm::Matrix world;
};
