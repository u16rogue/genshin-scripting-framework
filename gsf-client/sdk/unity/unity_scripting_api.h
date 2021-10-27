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

	template <typename ParentType, typename UnityType>
	class unity_setter_getter
	{
		unity_setter_getter(unity_setter_getter &) = delete;
		unity_setter_getter(const unity_setter_getter &) = delete;
		unity_setter_getter(unity_setter_getter &&) = delete;
	public:

		unity_setter_getter(ParentType *parent_, const char *getter_api_name, const char *setter_api_name)
			: parent(parent_), getter(getter_api_name), setter(setter_api_name) {}

		void operator=(UnityType &rhs) noexcept
		{
			this->setter(this->parent, rhs);
		}

		operator UnityType() noexcept
		{
			return this->getter(this->parent);
		}

	public:
		const ParentType *parent;
		unity_scripting_api<UnityType, ParentType *> getter;
		unity_scripting_api<void, ParentType *, UnityType> setter;
	};

	template <typename UnityType>
	class unity_setter_getter_static
	{
		unity_setter_getter_static(unity_setter_getter_static &) = delete;
		unity_setter_getter_static(const unity_setter_getter_static &) = delete;
		unity_setter_getter_static(unity_setter_getter_static &&) = delete;
	public:

		unity_setter_getter_static(const char *getter_api_name, const char *setter_api_name)
			: getter(getter_api_name), setter(setter_api_name) {}

		void operator=(UnityType rhs) noexcept
		{
			this->setter(rhs);
		}

		operator UnityType() noexcept
		{
			return this->getter();
		}

	public:
		unity_scripting_api<UnityType> getter;
		unity_scripting_api<void, UnityType> setter;
	};

}