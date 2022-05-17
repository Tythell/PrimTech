#pragma once
#include"../Math/Math.h"
struct Transform
{
	sm::Vector3 pos, rot, scale;
	sm::Matrix world;
};
