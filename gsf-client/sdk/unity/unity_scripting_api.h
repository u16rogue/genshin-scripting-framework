#pragma once

#include <macro.h>
#include <vector>
#include "../il2cpp/il2cpp.h"

namespace game::sdk
{
	class _unity_scripting_api
	{
		using get_unity_api_t = void *(*)(const char *);

	public:
		_unity_scripting_api(const char *name_)
			: name(name_)
		{
			if (!name_)
				return;

			this->_instances.emplace_back(this);
		}

		bool load_function()
		{
			bool result = DEBUG_CON_C_LOG(this->name, (this->function = this->get_unity_api(this->name)));

			#ifdef _DEBUG
			{
				if (result)
				{
					DEBUG_COUT(" @ 0x" << this->function);
				}
			}
			#endif

			return result;
		}

		inline static bool load_function_all()
		{
			bool is_success = true;
			for (const auto &api : _unity_scripting_api::_instances)
			{
				if (!api->load_function())
				{
					is_success = false;
					break;
				}
			}

			_unity_scripting_api::_instances.clear();
			return is_success;
		}

	protected:
		void *function = nullptr;
		const char *name;

	public:
		inline static get_unity_api_t get_unity_api = nullptr; // TODO: reverse this and then implement hashable version

	private:
		inline static std::vector<_unity_scripting_api *> _instances;
	};

	template <typename fn_ret_t = void, typename... fn_args_t>
	class unity_scripting_api : public _unity_scripting_api
	{
	public:

		using type = fn_ret_t(*)(fn_args_t...);

		unity_scripting_api(const char *name_)
			: _unity_scripting_api(name_)
		{};

		fn_ret_t operator()(fn_args_t... args)
		{
			return reinterpret_cast<type>(this->function)(args...);
		}

		void *api_ptr() const
		{
			return this->function;
		}
	};

	#if 0
	template <typename parent_type, typename getter_api_t, typename setter_api_t>
	class unity_setter_getter_bridge
	{
	public:
		unity_setter_getter_bridge()
			: {}
	};
	#endif

	template <typename unity_type>
	class unity_setter_getter_static
	{
		unity_setter_getter_static(unity_setter_getter_static &) = delete;
		unity_setter_getter_static(const unity_setter_getter_static &) = delete;
		unity_setter_getter_static(unity_setter_getter_static &&) = delete;
	public:

		unity_setter_getter_static(const char *getter_api_name, const char *setter_api_name)
			: getter(getter_api_name), setter(setter_api_name) {}

		void operator=(unity_type rhs) noexcept
		{
			this->setter(rhs);
		}

		operator unity_type() noexcept
		{
			return this->getter();
		}

		unity_type operator->() noexcept
		{
			return this->getter();
		}

	public:
		unity_scripting_api<unity_type> getter;
		unity_scripting_api<void, unity_type> setter;
	};
}

#define GSF_UNITY_SDK_ENSURE_NO_NONSTATIC(class_interface) static_assert(sizeof(class_interface) <= 1, "[ " #class_interface " ] WARNING: Unity class interface in GSF's unity sdk might have member values! Classes made for interfacing with unity must be pure static and must not contain non static member variables!")