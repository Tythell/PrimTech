#pragma once
#include "Camera.h"

struct cbTransformsQuad
{
	sm::Matrix world;
	sm::Matrix viewProj;
	sm::Vector4 color;
};

struct cbWorldTransforms3D
{
	sm::Matrix world;
	sm::Matrix viewProj;
};