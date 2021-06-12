#include "misc_utils.h"
#include <Windows.h>
#include <ctime>

void utils::spinlock_key(int key, unsigned int sleepms)
{
	while (!GetAsyncKeyState(key))
		Sleep(sleepms);
}

std::wstring utils::random_str(int length, std::wstring_view char_set)
{
	std::srand(std::time(nullptr));
	const std::size_t set_max_idx = char_set.length();
	std::wstring result;
	result.reserve(length + 1);

	for (int i = 0; i < length; i++)
		result += char_set[std::rand() % set_max_idx];

	return result;
}
