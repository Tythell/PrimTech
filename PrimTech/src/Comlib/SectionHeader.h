#pragma once
#include <DirectXMath.h>

struct SectionHeader
{
	Headers header;
	size_t messageLength;
	size_t messageID;
};

// Header for sending next message
struct MessageHeader
{
	char message[1024];
	float position[3];
};