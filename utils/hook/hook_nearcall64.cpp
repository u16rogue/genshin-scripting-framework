#pragma once

#if 0

#include <Windows.h>
#include <cstdint>
#include <cstddef>
#include "hook_nearcall64.h"

bool utils::hook_nearcall64(void *instruction_address, void *hook_address, void **original_address_out)
{
	constexpr std::ptrdiff_t instruction_size = 6; // 2 opcode - 4 offset
	std::uint8_t *inst_adr = reinterpret_cast<std::uint8_t*>(instruction_address);
	std::uint16_t *opcode  = reinterpret_cast<std::uint16_t*>(inst_adr);
	std::int32_t  *operand = reinterpret_cast<std::int32_t*>(inst_adr + 0x2);
	std::uintptr_t next_op = reinterpret_cast<std::uintptr_t>(instruction_address) + instruction_size;

	if ( !instruction_address || !hook_address )
		return false;

	// Since we're using an x86 near call jump instruction, check if the offset is within range for a 4 byte offset
	if ( reinterpret_cast<std::uintptr_t>(hook_address) - next_op > 0x00000000FFFFFFFF )
		return false;

	// Allow read, write, and execute
	DWORD o_prot = NULL;
	if ( !VirtualProtect(instruction_address, instruction_size, PAGE_EXECUTE_READWRITE, &o_prot) )
		return false;
	
	if ( *opcode == 0x15FF )
		*opcode = 0xE890;// Replace the x64 near call to an x86 near call
	else if ( *opcode == 0xE890 )
		*opcode = 0x15FF; // Should allow us to "rehook" the call instruction with the original address which basically is unhooking it
	else
		return false;

	// Return the original address. Calculated by adding the address of the next instruction to the call operand.
	if ( original_address_out )
	{
		*original_address_out = reinterpret_cast<void*>(next_op + *operand);

		// Check if the destination address contains a SUB RSP (48 83 EC) instruction, if it doesn't dereference the current original address unless it goes to a jump table.
		// NOTE: does not apply for every function
		// TODO: implement this properly for other functions
		if ( (*reinterpret_cast<std::uint32_t*>(hook_address) & 0x00FFFFFF) != 0x00EC8348 && *reinterpret_cast<std::uint8_t*>(hook_address) != 0xE9 )
			*original_address_out = *reinterpret_cast<void**>(*original_address_out);
	}

	// Hook the call by placing the offset of the hook address
	*operand = (reinterpret_cast<std::uintptr_t>(hook_address) - next_op) & 0xFFFFFFFF;

	// Restore the original protection
	if ( !VirtualProtect(instruction_address, instruction_size, o_prot, &o_prot) )
		return false;

	return true;
}

#endif