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
		float fadeout_duration  = 0.f;
		float duration          = 0.f;
		float duration_absolute = 0;
	};

	std::uint8_t *calc_rel_address_32(void *instruction_address, std::size_t instruction_size);
}