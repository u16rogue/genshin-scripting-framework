#include "hook.h"

#include <Windows.h>
#include <macro.h>

#include "winapi_helper.h"

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

utils::hook_base::hook_base()
{
	utils::hook_base::instances.push_back(this);
}

utils::hook_base::hook_base(void *target_, void *hookfn_)
	: target(target_), hookfn(hookfn_)
{
	utils::hook_base::instances.push_back(this);
}

utils::hook_base::~hook_base()
{
	this->unhook();
	utils::hook_base::instances.remove(this);
}

utils::hook_vmt::hook_vmt(int index_, void *hook_)
	: index(index_), utils::hook_base(nullptr, hook_)
{
}

bool utils::hook_vmt::init(void **vtable)
{
	this->vfunc_entry = vtable + this->index;
	return true;
}

bool utils::hook_vmt::hook()
{
	utils::change_page_protection page_prot_vfunc(this->vfunc_entry, sizeof(void*), PAGE_EXECUTE_READWRITE);
	if (!page_prot_vfunc)
		return false;

	this->originalfn = *this->vfunc_entry;
	*this->vfunc_entry = this->hookfn;

	return true;
}

bool utils::hook_vmt::unhook()
{
	utils::change_page_protection page_prot_vfunc(this->vfunc_entry, sizeof(void *), PAGE_EXECUTE_READWRITE);
	if (!page_prot_vfunc)
		return false;

	*this->vfunc_entry = this->originalfn;

	return true;
}