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

		void *get_ptr()
		{
			return this->function;
		}
	};
}