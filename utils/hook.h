#pragma once

namespace utils
{
	bool hook_vmt_swap(void **vtable, int index, void *hook_fn, void **out_orig_fn);
}