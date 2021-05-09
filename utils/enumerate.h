#pragma once

#include "winternal.h"

namespace utils
{
	using enumerate_modules_callback_t = bool(*)(void *handle, const wchar_t *modname);
	bool enumerate_modules(void *proc_handle, enumerate_modules_callback_t callback);

	ldr_data_table_entry *ldr_data_table_entry_find(const wchar_t *name);
	bool ldr_data_table_entry_find(const wchar_t *name, ldr_data_table_entry *&dest);
}