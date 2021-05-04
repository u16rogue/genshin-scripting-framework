#include "aob_scan.h"
#include <string>

std::uint8_t *utils::aob_scan(void *start, std::size_t size, const char *signature, const char *mask)
{
	std::size_t byte_count = 0;
	if ( !start || size < 1 || !signature || !mask || std::strlen(signature) != (byte_count = std::strlen(mask)) )
		return nullptr;


}