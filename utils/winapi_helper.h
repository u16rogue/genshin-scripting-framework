#pragma once

#include <Windows.h>

namespace utils
{
	class open_winthread
	{
	public:
		open_winthread(int thread_id);
		~open_winthread();

		operator bool() const
		{
			return !!thread_handle;
		}

		bool suspend();
		bool resume();

	private:
		HANDLE thread_handle = nullptr;
	};
}