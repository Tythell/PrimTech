#pragma once
#include <DirectXMath.h>

// Header for sending information about next message
struct MessageHeader
{
	uint header;
	size_t messageID;
	size_t messageLength;
};