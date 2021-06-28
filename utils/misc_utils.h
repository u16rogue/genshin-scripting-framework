#pragma once

#include <string>
#include <cstdint>

namespace utils
{
	void spinlock_key(int key = 0x20 /*VK_SPACE*/, unsigned int sleepms = 100);

	std::wstring random_str(int length = 12, std::wstring_view char_set = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

	class fader_float
	{
	public:
		fader_float(float fadeout_duration_, float duration_ = 0.f);

		void mark(float duration_ = 0.f);
		float get();

	private:
		float fadeout_duration = 0.f;
		float duration         = 0.f;
		
		std::uint64_t duration_absolute = 0;
	};

}