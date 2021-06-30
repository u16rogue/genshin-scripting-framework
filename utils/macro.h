#pragma once

#if defined(_DEBUG) && !defined(NDEBUG)
	#include <iostream>
	#define DEBUG_WCOUT(x) std::wcout << x
	#define DEBUG_COUT(x) std::cout << x
#elif defined(NDEBUG) && !defined(_DEBUG)
	#define DEBUG_WCOUT(x)
	#define DEBUG_COUT(x)
#endif

// Used for converting ascii constant strings defined as macros to unicode by appending an L (simple turns "hello" to L"hello" but works for macros too)
#define UTILS_A2W_MDEF(m) L"" ## m

#define GET_VFUNC_FROM_VTABLE_BY_IDX(table, index) reinterpret_cast<void**>(table)[static_cast<int>(index)]