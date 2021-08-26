#include "misc_utils.h"
#include <Windows.h>
#include <ctime>

void utils::spinlock_key(int key, unsigned int sleepms)
{
	while (!GetAsyncKeyState(key))
		Sleep(sleepms);
}

std::wstring utils::random_str(int length, std::wstring_view char_set)
{
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	const std::size_t set_max_idx = char_set.length();
	std::wstring result;
	result.reserve(length + 1);

	for (int i = 0; i < length; i++)
		result += char_set[std::rand() % set_max_idx];

	return result;
}

std::uint8_t *utils::calc_rel_address_32(void *instruction_address, std::size_t instruction_operand_relative_offset)
{
	std::int32_t offset = *reinterpret_cast<std::int32_t *>(reinterpret_cast<std::uint8_t *>(instruction_address) + instruction_operand_relative_offset);
	std::uintptr_t next_instruction = reinterpret_cast<std::uintptr_t>(instruction_address) + instruction_operand_relative_offset + sizeof(std::int32_t);
	return reinterpret_cast<std::uint8_t *>(next_instruction + offset);
}

#pragma warning (disable: 28159)

utils::fader_float::fader_float(float fadeout_duration_, float duration_)
	: fadeout_duration(fadeout_duration_), duration(duration_)
{
}

void utils::fader_float::mark(float duration_)
{
	if (duration_)
		this->duration = duration_;

	this->duration_absolute = static_cast<float>(GetTickCount()) + this->duration + this->fadeout_duration;
}

float utils::fader_float::get()
{
	return static_cast<float>(this->duration_absolute - GetTickCount()) / this->fadeout_duration;
}

#pragma warning (default: 28159)
