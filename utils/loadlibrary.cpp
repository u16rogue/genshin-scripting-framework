#include "loadlibrary.h"
#include <Windows.h>
#include "misc_utils.h"
#include "winapi_helper.h"
#include "winternal.h"
#include <filesystem>

using LoadLibraryA_t = decltype(LoadLibraryA) *;
using LoadLibraryW_t = decltype(LoadLibraryW) *;
using GetProcAddress_t = decltype(GetProcAddress) *;

struct map_module_info_t
{
	void *base_address = nullptr;
    LoadLibraryA_t   imp_LoadLibraryA = nullptr;
    GetProcAddress_t imp_GetProcAddress = nullptr;
};

//__declspec(dllexport)
//__declspec(noinline)
__declspec(safebuffers) /* disable GS */
DWORD __stdcall shellcode_map_module(map_module_info_t *info)
{
    if (!utils::pe_validate_dosheader(info->base_address))
        return -1;

    PIMAGE_NT_HEADERS nt_header = utils::pe_get_ntheaderptr(info->base_address);
    if (!nt_header)
        return -1;

    std::uint8_t *base_u8 = reinterpret_cast<std::uint8_t *>(info->base_address);

    // Relocations
    utils::unmapped_pe_relocate(info->base_address);

    // Function imports
    if (!utils::unmapped_pe_load_iat(info->base_address, info->imp_LoadLibraryA, info->imp_GetProcAddress))
        return -1;

    // Invoke entry point
    return reinterpret_cast<BOOL(APIENTRY*)(HMODULE, DWORD, LPVOID)>(base_u8 + nt_header->OptionalHeader.AddressOfEntryPoint)(reinterpret_cast<HMODULE>(info->base_address), DLL_PROCESS_ATTACH, nullptr);
}

void *utils::_debug_get_address_of_shellcode_map_module()
{
    return shellcode_map_module;
}

bool utils::map_module(void *proc_handle, void *thread_handle, void *module_bin, std::size_t bin_size)
{
    // Suspend thread
	if (thread_handle && SuspendThread(thread_handle) == 0xFFFFFF)
        return false;

    PIMAGE_NT_HEADERS nt_header      = utils::pe_get_ntheaderptr(module_bin);
	std::size_t       shellcode_size = 4096; // utils::unsafe_get_subroutine_size(shellcode_map_module);

	if (!shellcode_size)
		return false;

    // Allocate space for module
	utils::remote_allocate proc_alloc(proc_handle, nt_header->OptionalHeader.SizeOfImage, PAGE_EXECUTE_READWRITE);
	if (!proc_alloc)
		return false;

	std::uint8_t *module_bin_u8 = reinterpret_cast<std::uint8_t *>(module_bin);
	std::uint8_t *proc_alloc_u8 = reinterpret_cast<std::uint8_t *>(proc_alloc.ptr);

    // Write header
	if (!WriteProcessMemory(proc_handle, proc_alloc.ptr, module_bin, nt_header->OptionalHeader.SizeOfHeaders, nullptr))
		return false;

    // Write sections
	PIMAGE_SECTION_HEADER payload_sections = reinterpret_cast<PIMAGE_SECTION_HEADER>(nt_header + 1);
	for (int i = 0; i < nt_header->FileHeader.NumberOfSections; i++)
		if (!WriteProcessMemory(proc_handle, proc_alloc_u8 + payload_sections[i].VirtualAddress, module_bin_u8 + payload_sections[i].PointerToRawData, payload_sections[i].SizeOfRawData, nullptr))
			return false;

    // Allocate space for shellcode
    utils::remote_allocate sh_alloc(proc_handle, shellcode_size + sizeof(map_module_info_t), PAGE_EXECUTE_READWRITE);
    if (!sh_alloc)
        return false;

    std::uint8_t *sh_local = reinterpret_cast<std::uint8_t*>(shellcode_map_module);
    // check if the compiler placed it in a jump table
    if (sh_local[0] == 0xE9)
        sh_local = reinterpret_cast<std::uint8_t*>((sh_local + 5) + *reinterpret_cast<std::int32_t*>(sh_local + 1));

    // Write shellcode
    if (!WriteProcessMemory(proc_handle,sh_alloc.ptr, sh_local, shellcode_size, nullptr))
        return false;

    // Write info
    HMODULE kernel32 = GetModuleHandleW(L"Kernel32.dll");
    if (!kernel32)
        return false;

    map_module_info_t *info_alloc = reinterpret_cast<map_module_info_t *>(reinterpret_cast<std::uint8_t*>(sh_alloc.ptr) + shellcode_size);
    map_module_info_t info {};
    info.base_address = proc_alloc.ptr;
    info.imp_LoadLibraryA = reinterpret_cast<LoadLibraryA_t>(GetProcAddress(kernel32, "LoadLibraryA"));
    info.imp_GetProcAddress = reinterpret_cast<GetProcAddress_t>(GetProcAddress(kernel32, "GetProcAddress"));

    if (!info.imp_GetProcAddress || !info.imp_LoadLibraryA)
        return false;

    if (!WriteProcessMemory(proc_handle, info_alloc, &info, sizeof(info), nullptr))
        return false;

    // Execute shellcode
    utils::remote_execute sh_exec(proc_handle, sh_alloc.ptr, info_alloc);
    if (!sh_exec.execute())
        return false;

    // Resume thread
    if (thread_handle && ResumeThread(thread_handle) == 0xFFFFFF)
        return false;

    sh_exec.wait();
    proc_alloc.leak();

	return true;
}

bool utils::remote_loadlibrary(void *proc_handle, std::wstring_view path)
{
    if (proc_handle == nullptr || path.empty() || !std::filesystem::exists(path))
        return false;

    std::size_t buffer_len = (path.length() + 1) * sizeof(wchar_t);

    utils::remote_allocate str_buffer(proc_handle, buffer_len);
    if (!str_buffer)
        return false;

    if (!WriteProcessMemory(proc_handle, str_buffer.ptr, path.data(), buffer_len, nullptr))
        return false;

    HMODULE kernel32 = GetModuleHandleW(L"Kernel32.dll");
    if (!kernel32)
        return false;

    LoadLibraryW_t llw = reinterpret_cast<LoadLibraryW_t>(GetProcAddress(kernel32, "LoadLibraryW"));
    if (!llw)
        return false;

    utils::remote_execute llw_exec(proc_handle, llw, str_buffer.ptr);
    if (!llw_exec.execute())
        return false;

    llw_exec.wait();

    return true;
}
