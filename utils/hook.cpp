#include "hook.h"

#include <Windows.h>
#include <macro.h>

bool utils::hook_vmt_swap(void **vtable, int index, void *hook_fn, void **out_orig_fn)
{
	void ** const vfunc_entry = vtable + index;

	DWORD o_prot = 0;
	if (!VirtualProtect(vfunc_entry, sizeof(void *), PAGE_EXECUTE_READWRITE, &o_prot))
		return false;

	if (out_orig_fn)
		*out_orig_fn = *vfunc_entry;
	
	*vfunc_entry = hook_fn;

	if (!VirtualProtect(vfunc_entry, sizeof(void *), o_prot, &o_prot))
		return false;

	return true;
}