#pragma once
#include "Camera.h"

struct cbTransforms
{
	sm::Matrix world;
	sm::Matrix viewProj;
	sm::Vector4 color;
};