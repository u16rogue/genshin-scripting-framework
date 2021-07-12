#include "pattern_scan.h"
#include <string>
#include <memory>
#include <Windows.h>

std::uint8_t *utils::aob_scan(void *start, std::size_t size, const char *signature, const char *mask)
{
	if ( !start || size < 1 || !signature || !mask )
		return nullptr;

	std::uint8_t      *current_address = reinterpret_cast<std::uint8_t *>(start);
	const std::size_t  byte_count      = std::strlen(mask);
	const std::uint8_t *end            = current_address + size;

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
	}
	while (++current_address + byte_count <= end);

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

	return reinterpret_cast<std::uint8_t*>(start) + (reinterpret_cast<std::uintptr_t>(result) - reinterpret_cast<std::uintptr_t>(buffer.get()));
}

std::uint8_t *utils::ida_scan(void *start, std::size_t size, const char *signature)
{
	return nullptr;
}
