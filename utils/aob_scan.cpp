#include "aob_scan.h"
#include <string>

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