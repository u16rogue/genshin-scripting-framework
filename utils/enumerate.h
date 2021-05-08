#pragma once

namespace utils
{
	using enumerate_modules_callback_t = bool(*)(void *handle, const wchar_t *modname);
	bool enumerate_modules(void *proc_handle, enumerate_modules_callback_t callback);
}