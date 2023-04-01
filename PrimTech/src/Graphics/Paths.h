#pragma once
#include <wincodec.h>

#ifdef _DEBUG

#define SHADER_DIR "../bin/Debug/"

#else

#define SHADER_DIR ((IsDebuggerPresent() == true)) ? std::string("../bin/Release/") : ""

#endif // _DEBUG