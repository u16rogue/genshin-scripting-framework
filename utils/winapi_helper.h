#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstddef>
#include <string>

namespace utils
{
	using enumerate_modules_callback_t = bool(*)(void *handle, const wchar_t *modname);
	bool enumerate_modules(void *proc_handle, enumerate_modules_callback_t callback);

	class open_winthread
	{
	public:
		open_winthread(DWORD thread_id);
		~open_winthread();

		operator bool() const
		{
			return this->thread_handle != nullptr;
		}

		bool suspend();
		bool resume();

		const DWORD id = 0;

	private:
		HANDLE thread_handle = nullptr;
	};

	class remote_allocate
	{
	public:
		remote_allocate() = default;
		remote_allocate(HANDLE proc_handle_, std::size_t alloc_size_, DWORD page_prot_ = PAGE_READWRITE);
		~remote_allocate();

		operator bool() const
		{
			return this->ptr != nullptr;
		}

		bool alloc(HANDLE proc_handle_ = nullptr, std::size_t alloc_size_ = 0, DWORD page_prot_ = NULL);
		bool free();
		void leak();

		LPVOID ptr = nullptr;

	private:
		HANDLE      proc_handle = nullptr;
		std::size_t alloc_size  = 0;
		DWORD       page_prot   = PAGE_READWRITE;
	};

	class remote_execute
	{
	public:
		remote_execute() = default;
		remote_execute(HANDLE proc_handle_, LPVOID routine_adr_, void *arg_ptr_ = nullptr);
		~remote_execute();

		bool execute(HANDLE proc_handle_ = nullptr, LPVOID routine_adr_ = nullptr, void *arg_ptr_ = nullptr);
		void wait(DWORD timeout = INFINITE);
		bool execute_wait(DWORD timeout = INFINITE, HANDLE proc_handle_ = nullptr, LPVOID routine_adr_ = nullptr, void *arg_ptr_ = nullptr);

	private:
		HANDLE  proc_handle = nullptr;
		LPVOID  routine_adr = nullptr;
		void   *arg_ptr     = nullptr;
		HANDLE  crt_handle  = nullptr;
	};

	class change_page_protection
	{
	public:
		change_page_protection(void *address_, std::size_t size_, DWORD new_page_protection);
		~change_page_protection();

		operator bool() const
		{
			return this->is_success;
		}

	private:
		void *address    = nullptr;
		std::size_t size = 0;
		DWORD old_prot   = 0;
		bool is_success  = false;
	};

	std::wstring get_full_path(std::wstring_view initial_path);
}