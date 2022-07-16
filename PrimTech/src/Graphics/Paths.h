#pragma once
#include <wincodec.h>

#ifdef _DEBUG

#define SHADER_DIR "../x64/Debug/"

#else

#define SHADER_DIR ((IsDebuggerPresent() == true)) ? std::string("../x64/Release/") : ""

#endif // _DEBUG