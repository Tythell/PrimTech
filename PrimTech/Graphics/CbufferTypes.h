#pragma once
#include "Camera.h"

struct Transforms
{
	sm::Matrix world;
	sm::Matrix viewProj;
	sm::Vector4 color;
};