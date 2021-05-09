#include "misc_utils.h"
#include <Windows.h>

void utils::spinlock_key(int key, unsigned int sleepms)
{
	while (!GetAsyncKeyState(key))
		Sleep(sleepms);
}