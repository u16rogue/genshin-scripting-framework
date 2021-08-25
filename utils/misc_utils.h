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

	// REMARK: instruction_operand_relative_offset - This is the offset relative to instruction_address for where the relative address operand starts.
	// ex. jmp 0x1 which is equivalent to 0xE9 0x01 0x00 0x00 0x00 the offset in this case is 1 since the rel offset is after the E9 which is only 1 byte then the rest of the 4 bytes is the operand / rel offset
	std::uint8_t *calc_rel_address_32(void *instruction_address, std::size_t instruction_size, std::size_t instruction_operand_relative_offset);
}