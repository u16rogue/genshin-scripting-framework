#include <enumerate.h>
#include <Windows.h>
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