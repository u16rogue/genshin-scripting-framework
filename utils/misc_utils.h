#pragma once

namespace utils
{
	void spinlock_key(int key = 0x20 /*VK_SPACE*/, unsigned int sleepms = 100);
}