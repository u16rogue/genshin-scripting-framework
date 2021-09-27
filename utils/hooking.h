#pragma once

#include <memory>
#include <list>

namespace utils
{
	bool hook_vmt_swap(void **vtable, int index, void *hook_fn, void **out_orig_fn);

	class hook_base
	{
	public:
		hook_base();
		hook_base(void *target_, void *hookfn_);

		virtual ~hook_base();

		virtual bool hook()   { return false; };
		virtual bool unhook() { return false; };

		template <typename T>
		T *get_original()
		{
			return reinterpret_cast<T *>(this->originalfn);
		}

	private:

		void internal_on_unhook();

		void *target     = nullptr;
		void *originalfn = nullptr;
		void *hookfn     = nullptr;

		friend class hook_vmt;
		friend class hook_wndproc;
		friend class hook_detour;

	public:
		inline static std::list<utils::hook_base *> instances;
	};

	class hook_detour : public hook_base
	{
	public:
		hook_detour(void *hookfn_);

		bool init(void *target_);

		bool hook() override;
		bool unhook() override;
		bool inhook(void *target_);
	};

	class hook_vmt : public hook_base
	{
	public:
		hook_vmt(int index_, void *hook_);

		bool init(void **vtable);

		bool hook() override;
		bool unhook() override;

	private:
		int index;
		void **vfunc_entry = nullptr;
	};

	class hook_wndproc : public hook_base
	{
	public:
		hook_wndproc(void *hookfn_);

		bool init(void *window_handle_);

		bool hook() override;
		bool unhook() override;
		bool inhook(void *target_);

	private:
		void *window_handle = nullptr;
	};

}