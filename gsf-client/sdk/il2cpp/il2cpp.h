#pragma once
#include <cstdint>

#pragma warning(disable: 4200)

namespace game::sdk
{
	constexpr auto IL2CPP_ZERO_LEN_ARRAY = 0;

	using Il2CppObject = void *;
	using Il2CppChar = wchar_t;

	// TODO: use proper type
	using Il2CppClass = void;
	using Il2CppArray = void;

	struct Il2CppString
	{
		Il2CppObject  object;
		std::int32_t  length;
		Il2CppChar    chars[IL2CPP_ZERO_LEN_ARRAY];
	};

	using il2cpp_string_length_t = std::int32_t   (*)(Il2CppString *);
	using il2cpp_string_chars_t  = Il2CppChar    *(*)(Il2CppString *);
	using il2cpp_string_new_t    = Il2CppString  *(*)(const char *);

	using il2cpp_array_element_size_t = int(*)(const Il2CppClass *);
	using il2cpp_array_length_t       = std::uint32_t(*)(Il2CppArray *);
}

#pragma warning(default: 4200)