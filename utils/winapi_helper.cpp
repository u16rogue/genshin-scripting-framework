#include "winapi_helper.h"
#include <Psapi.h>

bool utils::enumerate_modules(void *proc_handle, enumerate_modules_callback_t callback)
{
	if (!proc_handle)
		return false;

	HMODULE proc_modules[1024] = { nullptr };
	DWORD module_size_total = NULL;

	if (!EnumProcessModules(proc_handle, proc_modules, sizeof(proc_modules), &module_size_total))
		return false;

	for (int i = 0; i < module_size_total / sizeof(HMODULE); i++)
	{
		wchar_t module_name[MAX_PATH] = { L'\0' };
		if (GetModuleFileNameEx(proc_handle, proc_modules[i], module_name, sizeof(module_name) / sizeof(wchar_t)))
			if (!callback(proc_modules[i], module_name))
				break;
	}

	return true;
}

utils::open_winthread::open_winthread(DWORD thread_id)
	: id(thread_id)
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

utils::remote_allocate::remote_allocate(HANDLE proc_handle_, std::size_t alloc_size_, DWORD page_prot_)
	: proc_handle(proc_handle_), alloc_size(alloc_size_), page_prot(page_prot_)
{
	this->alloc();
}

utils::remote_allocate::~remote_allocate()
{
	if (this->ptr)
		this->free();
}

bool utils::remote_allocate::alloc(HANDLE proc_handle_, std::size_t alloc_size_, DWORD page_prot_)
{
	if (proc_handle_)
		this->proc_handle = proc_handle_;

	if (alloc_size_)
		this->alloc_size = alloc_size_;

	if (page_prot_)
		this->page_prot = page_prot_;

	this->ptr = VirtualAllocEx(proc_handle, nullptr, alloc_size, MEM_COMMIT | MEM_RESERVE, page_prot);
	return this->ptr != nullptr;
}

bool utils::remote_allocate::free()
{
	if (this->proc_handle && this->ptr && VirtualFreeEx(proc_handle, this->ptr, NULL, MEM_RELEASE))
	{
		this->ptr = nullptr;
		return true;
	}
	
	return false;
}

void utils::remote_allocate::leak()
{
	this->proc_handle = nullptr;
}

std::wstring utils::get_full_path(std::wstring_view initial_path)
{
	wchar_t path_buffer[MAX_PATH] = { '\0' };

	if (!GetFullPathNameW(initial_path.data(), sizeof(path_buffer) / sizeof(wchar_t), path_buffer, nullptr))
		return std::wstring();

	return std::wstring(path_buffer);
}

utils::remote_execute::remote_execute(HANDLE proc_handle_, LPVOID routine_adr_, void *arg_ptr_)
	: proc_handle(proc_handle_), routine_adr(routine_adr_), arg_ptr(arg_ptr_)
{
}

utils::remote_execute::~remote_execute()
{
	if (this->crt_handle)
	{
		CloseHandle(this->crt_handle);
		this->crt_handle = nullptr;
	}
}

bool utils::remote_execute::execute(HANDLE proc_handle_, LPVOID routine_adr_, void *arg_ptr_)
{
	if (proc_handle_)
		this->proc_handle = proc_handle_;

	if (routine_adr_)
		this->routine_adr = routine_adr_;

	if (arg_ptr_)
		this->arg_ptr = arg_ptr_;

	this->crt_handle = CreateRemoteThread(this->proc_handle, nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(this->routine_adr), this->arg_ptr, NULL, nullptr);
	return this->crt_handle != nullptr;
}

void utils::remote_execute::wait(DWORD timeout)
{
	if (this->crt_handle)
		WaitForSingleObject(this->crt_handle, timeout);
}

bool utils::remote_execute::execute_wait(DWORD timeout, HANDLE proc_handle_, LPVOID routine_adr_, void *arg_ptr_)
{
	this->execute(proc_handle_, routine_adr_, arg_ptr_);
	this->wait(timeout);

	return this->crt_handle != nullptr;
}

utils::change_page_protection::change_page_protection(void *address_, std::size_t size_, DWORD new_page_protection)
{
	this->is_success = VirtualProtect(address_, size_, new_page_protection, &this->old_prot);
}

utils::change_page_protection::~change_page_protection()
{
	VirtualProtect(this->address, this->size, this->old_prot, &this->old_prot);
}
