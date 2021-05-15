#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

namespace utils
{
	void *_debug_get_address_of_shellcode_map_module();
	bool map_module(void *proc_handle, void *thread_handle, void *module_bin, std::size_t bin_size);
	bool remote_loadlibrary(void *proc_handle, std::wstring_view path);
}