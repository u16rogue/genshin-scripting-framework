#pragma once

#include <cstdint>
#include <type_traits>

namespace utils
{
	using fnv1a64_t = std::uint64_t;
	using fnv1a32_t = std::uint32_t;

	template <typename hash_size = utils::fnv1a64_t, typename string_t>
	constexpr hash_size hash_fnv1a(const string_t *string)
	{
		static_assert(std::is_same<hash_size, utils::fnv1a64_t>::value || std::is_same<hash_size, utils::fnv1a32_t>::value, "Invalid hash size type! Only supported types: utils::fnv1a64_t and utils::fnv1a32_t");

		constexpr hash_size prime = std::is_same<hash_size, std::uint64_t>::value ? 0x00000100000001B3 : 0x01000193;
		hash_size result          = std::is_same<hash_size, std::uint64_t>::value ? 0xcbf29ce484222325 : 0x811c9dc5;

		while (*string)
		{
			result = (result ^ *string) * prime;
			++string;
		}

		return result;
	}

	template <typename hash_size = utils::fnv1a64_t, typename string_t>
	consteval hash_size hash_fnv1a_cv(const string_t *string)
	{
		return utils::hash_fnv1a<hash_size>(string);
	}
}