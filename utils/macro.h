#pragma once

#if defined(_DEBUG) && !defined(NDEBUG)
	#include <iostream>
	#define DEBUG_WCOUT(x) std::wcout << x
#elif defined(NDEBUG) && !defined(_DEBUG)
	#define DEBUG_WCOUT(x)
#endif