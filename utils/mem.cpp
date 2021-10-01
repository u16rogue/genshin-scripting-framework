#include "mem.h"
#include <string>
#include <memory>
#include <Windows.h>
#include <vector>

std::uint8_t *utils::aob_scan(void *start, std::size_t size, const char *signature, const char *mask)
{
	if (!start || size < 1 || !signature || !mask)
		return nullptr;

	std::uint8_t *current_address = reinterpret_cast<std::uint8_t *>(start);
	const std::size_t  byte_count = std::strlen(mask);
	const std::uint8_t *end = current_address + size;

	do
	{
		for (int i = 0; i <= byte_count; i++)
		{
			if (mask[i] == '?')
				continue;
			else if (mask[i] != 'x' || current_address[i] != reinterpret_cast<const unsigned char *>(signature)[i])
				break;
			else if (mask[i + 1] == '\0')
				return current_address;
		}
	} 	while (++current_address + byte_count <= end);

	return nullptr;
}

std::uint8_t *utils::aob_scan(void *proc_handle, void *start, std::size_t size, const char *signature, const char *mask)
{
	std::unique_ptr<std::uint8_t[]> buffer = std::make_unique<std::uint8_t[]>(size);
	if (!ReadProcessMemory(proc_handle, start, buffer.get(), size, nullptr))
		return nullptr;

	std::uint8_t *result = utils::aob_scan(buffer.get(), size, signature, mask);
	if (!result)
		return nullptr;

	return reinterpret_cast<std::uint8_t *>(start) + (reinterpret_cast<std::uintptr_t>(result) - reinterpret_cast<std::uintptr_t>(buffer.get()));
}

constexpr std::uint8_t INVALID_NIBBLE = 0xF0;
std::uint8_t hex_char_to_nibble(const char cnibble)
{
	if (cnibble >= '0' && cnibble <= '9')
		return static_cast<std::uint8_t>(cnibble - '0');
	else if (cnibble >= 'A' && cnibble <= 'F')
		return static_cast<std::uint8_t>((cnibble - 'A') + 0xA);
	else if (cnibble >= 'a' && cnibble <= 'f')
		return static_cast<std::uint8_t>((cnibble - 'a') + 0xa);

	return INVALID_NIBBLE;
}

bool hex_str_to_byte(std::uint8_t &byte_out, const char strhex[2])
{
	auto upper_nibble = hex_char_to_nibble(strhex[0]);
	auto lower_nibble = hex_char_to_nibble(strhex[1]);

	if (upper_nibble == INVALID_NIBBLE || lower_nibble == INVALID_NIBBLE)
		return false;

	byte_out = (upper_nibble << 4) | lower_nibble;
	return true;
}

std::uint8_t *utils::ida_scan(void *start, std::size_t size, const char *signature)
{
	// is this lazy? just creating data from an ida pattern for the aob scanner.
	// perhaps i could argue that atleast you only have to parse the ida pattern once instead of doing it everytime you check for sigs.

	std::vector<std::uint8_t> aob;
	std::string mask;

	do
	{
		if (*signature == ' ')
			continue;

		if (*signature == '?')
		{
			aob.push_back(0);
			mask.append("?");
			continue;
		}

		std::uint8_t byte_result = 0;

		if (!hex_str_to_byte(byte_result, signature))
			return nullptr;

		aob.push_back(byte_result);
		mask.append("x");
		if (*++signature == '\0') // skip the second nibble, the if statement is for ensuring that we dont go oob incase of a bad signature (all sig should be a pair, this will be caught by hex_str_to_byte anyway but just incase, you'll never know!)
			return nullptr;

	} while (*++signature != '\0');

	if (aob.size() != mask.length())
		return nullptr; // this should throw an exception instead

	return utils::aob_scan(start, size, reinterpret_cast<const char *>(aob.data()), mask.c_str());
}

std::uint8_t *utils::calc_rel2abs32(void *instruction_address, std::size_t instruction_size)
{
	auto next_inst = reinterpret_cast<std::uintptr_t>(instruction_address) + instruction_size;
	return reinterpret_cast<std::uint8_t *>(next_inst + *reinterpret_cast<std::int32_t *>((next_inst - sizeof(std::uint32_t))));
}