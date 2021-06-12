#pragma once

#include <string>

namespace utils
{
	void spinlock_key(int key = 0x20 /*VK_SPACE*/, unsigned int sleepms = 100);

	std::wstring random_str(int length = 12, std::wstring_view char_set = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
}