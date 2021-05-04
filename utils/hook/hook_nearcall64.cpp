#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstddef>
#include "../hook.h"

bool utils::hook_nearcall64(void *instruction_address, void *hook_address, void **original_address_out)
{
	constexpr std::ptrdiff_t instruction_size = 6; // 2 opcode - 4 offset
	std::uint8_t *inst_adr = reinterpret_cast<std::uint8_t *>(instruction_address);
	std::uint16_t *opcode  = reinterpret_cast<std::uint16_t *>(inst_adr);
	std::int32_t  *operand = reinterpret_cast<std::int32_t *>(inst_adr + 0x2);
	std::uintptr_t next_op = reinterpret_cast<std::uintptr_t>(instruction_address) + instruction_size;

	if ( !instruction_address || !hook_address )
		return false;

	// Allow read, write, and execute
	DWORD o_prot = NULL;
	if ( !VirtualProtect(instruction_address, instruction_size, PAGE_EXECUTE_READWRITE, &o_prot) )
		return false;

	// Check x64 near call opcode (FF 15)
	if ( *opcode != 0x15FF )
		return false;

	// Return the original address. Calculated by adding the address of the next instruction to the call operand.
	if ( original_address_out )
		*original_address_out = reinterpret_cast<void *>(next_op + *operand);

	// Hook the call by placing the offset of the hook address
	*operand = reinterpret_cast<std::int32_t>(reinterpret_cast<std::uint8_t *>(hook_address) - next_op);

	// Restore the original protection
	if ( !VirtualProtect(instruction_address, instruction_size, o_prot, &o_prot) )
		return false;

	return true;
}