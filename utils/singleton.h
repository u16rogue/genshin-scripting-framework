#pragma once

namespace utils
{
	template <typename T>
	class impl_singleton
	{
	public:
		static T &si_get()
		{
			return impl_singleton_instance;
		}

	private:
		static T impl_singleton_instance;
	};

	template <typename T>
	inline T impl_singleton<T>::impl_singleton_instance;
}