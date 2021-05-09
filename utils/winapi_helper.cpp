#include "winapi_helper.h"

utils::open_winthread::open_winthread(int thread_id)
{
	this->thread_handle = OpenThread(THREAD_ALL_ACCESS, false, thread_id);
}

utils::open_winthread::~open_winthread()
{
	if (this->thread_handle)
		CloseHandle(this->thread_handle);
}

bool utils::open_winthread::suspend()
{
	return SuspendThread(this->thread_handle) != 0xFFFFFFFF;
}

bool utils::open_winthread::resume()
{
	return ResumeThread(this->thread_handle) != 0xFFFFFFFF;
}