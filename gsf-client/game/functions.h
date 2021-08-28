#pragma once

namespace game
{
	inline void(*get_object)(const char *name) = nullptr; // UserAssembly.dll - \x48\x8b\xc4\x48\x89\x48\x00\x55\x41\x54 xxxxxx?xxx
}