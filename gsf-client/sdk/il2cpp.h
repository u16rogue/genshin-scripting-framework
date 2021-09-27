#pragma once
#include <cstdint>

#pragma warning(disable: 4200)

namespace game::sdk
{
	constexpr auto IL2CPP_ZERO_LEN_ARRAY = 0;

	using Il2CppObject = void *;
	using Il2CppChar = wchar_t;

	struct Il2CppString
	{
		Il2CppObject  object;
		std::int32_t  length;
		Il2CppChar    chars[IL2CPP_ZERO_LEN_ARRAY];
	};

	using il2cpp_string_length_t = std::int32_t   (*)(Il2CppString *);
	using il2cpp_string_chars_t  = Il2CppChar    *(*)(Il2CppString *);
	using il2cpp_string_new_t    = Il2CppString  *(*)(const char *);
}

#pragma warning(default: 4200)