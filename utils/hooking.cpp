#include "hooking.h"

#include <Windows.h>
#include <MinHook.h>
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
	// this->unhook();
}

void utils::hook_base::internal_on_unhook()
{
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

	this->internal_on_unhook();

	return true;
}

utils::hook_wndproc::hook_wndproc(void *hookfn_)
	: utils::hook_base(nullptr, hookfn_)
{
}

bool utils::hook_wndproc::init(void *window_handle_)
{
	this->window_handle = window_handle_;
	return true;
}

bool utils::hook_wndproc::hook()
{
	this->originalfn = reinterpret_cast<void *>(SetWindowLongPtrW(reinterpret_cast<HWND>(this->window_handle), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(this->hookfn)));
	return this->originalfn != nullptr;
}

bool utils::hook_wndproc::unhook()
{
	this->internal_on_unhook();
	return SetWindowLongPtrW(reinterpret_cast<HWND>(this->window_handle), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(this->originalfn)) != NULL;
}

utils::hook_detour::hook_detour(void *hookfn_)
	: utils::hook_base(nullptr, hookfn_)
{
}

bool utils::hook_detour::init(void *target_)
{
	this->target = target_;
	return MH_CreateHook(target_, this->hookfn, reinterpret_cast<LPVOID *>(&this->originalfn)) == MH_OK;
}

bool utils::hook_detour::hook()
{
	return MH_EnableHook(this->target) == MH_OK;
}

bool utils::hook_detour::unhook()
{
	this->internal_on_unhook();
	return MH_DisableHook(this->target) == MH_OK;
}
