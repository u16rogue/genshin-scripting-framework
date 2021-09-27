#pragma once

#include <cstdint>
#include <Windows.h>
#include <winternl.h>
#include <cstddef>
#include <memory>
#include <intrin.h>
#include "hash.h"

#define _WINTERNAL_INLINE __forceinline

namespace utils
{
	struct ldr_data_table_entry
	{
	public:
		ldr_data_table_entry *next;
	private:
		std::uintptr_t pad0[5];
	public:
		std::uint8_t *dll_base;
	private:
		std::uintptr_t pad1;
	public:
		std::uintptr_t size_of_image;
	private:
		std::uint16_t pad2[2];
	public:
		const wchar_t *full_dll_name;
	private:
		std::uint16_t pad3[2];
	public:
		const wchar_t *base_dll_name;
	};

    struct peb_ldr_data
    {
    private:
        std::uint32_t  pad0[2];
        std::uintptr_t pad1;
    public:
        utils::ldr_data_table_entry *entry;
    };

    struct peb
    {
    private:
        std::uint8_t   pad0[2];
    public:
        bool           being_debugged;
    private:
        std::uint8_t   pad1[16];
    public:
        peb_ldr_data *ldr;
    private:
        std::uint8_t   pad2[156];
    public:
        std::uint32_t  nt_global_flag;
    };

    _WINTERNAL_INLINE bool ldr_data_table_entry_next(utils::ldr_data_table_entry *&dest)
    {
        utils::ldr_data_table_entry *first_entry = reinterpret_cast<peb *>(__readgsqword(0x60))->ldr->entry;

        if (!dest)
        {
            if (!first_entry)
                return false;

            dest = first_entry;
            return true;
        }

        if (dest->next == nullptr || dest->next == first_entry)
            return false;

        dest = dest->next;
        return true;
    }

    _WINTERNAL_INLINE bool pe_validate_dosheader(void *base)
    {
        return reinterpret_cast<PIMAGE_DOS_HEADER>(base)->e_magic == IMAGE_DOS_SIGNATURE;
    }

    _WINTERNAL_INLINE PIMAGE_DOS_HEADER pe_get_dosheaderptr(void *base)
    {
        return reinterpret_cast<PIMAGE_DOS_HEADER>(base);
    }

    _WINTERNAL_INLINE PIMAGE_NT_HEADERS pe_get_ntheaderptr(void *base)
    {
        return reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uintptr_t>(base) + utils::pe_get_dosheaderptr(base)->e_lfanew);
    }

    _WINTERNAL_INLINE utils::ldr_data_table_entry *ldr_data_table_entry_find(const utils::fnv1a64_t hashed_name)
    {
        utils::ldr_data_table_entry *entry = nullptr;
        while (utils::ldr_data_table_entry_next(entry))
        {
            if (!entry->dll_base)
                continue;

            if (utils::hash_fnv1a(entry->base_dll_name) == hashed_name)
                return entry;
        }

        return nullptr;
    }

    _WINTERNAL_INLINE utils::ldr_data_table_entry *ldr_data_table_entry_find(const wchar_t *name)
    {
        return utils::ldr_data_table_entry_find(utils::hash_fnv1a(name));
    }

    _WINTERNAL_INLINE bool ldr_data_table_entry_find(const utils::fnv1a64_t hashed_name, utils::ldr_data_table_entry *&dest)
    {
        dest = utils::ldr_data_table_entry_find(hashed_name);

        if (!dest)
            return false;

        return true;
    }

    _WINTERNAL_INLINE bool ldr_data_table_entry_find(const wchar_t *name, utils::ldr_data_table_entry *&dest)
    {
        return utils::ldr_data_table_entry_find(utils::hash_fnv1a(name), dest);
    }

    _WINTERNAL_INLINE void *export_fn_get(void *mod_base, const utils::fnv1a64_t hashed_name)
    {
        if (!mod_base || !utils::pe_validate_dosheader(mod_base))
            return nullptr;

        PIMAGE_NT_HEADERS nt_header = utils::pe_get_ntheaderptr(mod_base);
        if (!nt_header)
            return nullptr;

        std::uint8_t *base = reinterpret_cast<std::uint8_t *>(mod_base);
        PIMAGE_EXPORT_DIRECTORY  image_export = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(base + nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
        std::uint32_t *names = reinterpret_cast<std::uint32_t *>(base + image_export->AddressOfNames);
        std::uint32_t *address = reinterpret_cast<std::uint32_t *>(base + image_export->AddressOfFunctions);
        std::uint16_t *ordinals = reinterpret_cast<std::uint16_t *>(base + image_export->AddressOfNameOrdinals);

        for (std::size_t i = 0; i < image_export->NumberOfFunctions; i++)
        {
            if (utils::hash_fnv1a(reinterpret_cast<const char *>(base + names[i])) == hashed_name)
                return base + (address[ordinals[i]]);
        }
        return nullptr;
    }

    _WINTERNAL_INLINE void *export_fn_get(void *mod_base, const char *name)
    {
        return utils::export_fn_get(mod_base, utils::hash_fnv1a(name));
    }

    _WINTERNAL_INLINE void *export_fn_find(const utils::fnv1a64_t hashed_name)
    {
        utils::ldr_data_table_entry *entry = nullptr;

        while (utils::ldr_data_table_entry_next(entry))
        {
            if (!entry->dll_base)
                continue;

            if (void *result = utils::export_fn_get(entry->dll_base, hashed_name); result)
                return result;
        }

        return nullptr;
    }

    _WINTERNAL_INLINE void *export_fn_find(const char *name)
    {
        return utils::export_fn_find(utils::hash_fnv1a(name));
    }

    _WINTERNAL_INLINE PIMAGE_BASE_RELOCATION base_relocation_block_forward(PIMAGE_BASE_RELOCATION current_block)
    {
        return reinterpret_cast<PIMAGE_BASE_RELOCATION>(reinterpret_cast<std::uintptr_t>(current_block) + current_block->SizeOfBlock);
    }

    _WINTERNAL_INLINE void unmapped_pe_relocate(void *base)
    {
        std::uint8_t      *base_u8   = reinterpret_cast<std::uint8_t *>(base);
        PIMAGE_NT_HEADERS  nt_header = utils::pe_get_ntheaderptr(base);

        #pragma warning (disable: 4311 4302)
        std::uint32_t      delta     = reinterpret_cast<std::uint32_t>(base_u8 - nt_header->OptionalHeader.ImageBase);
        #pragma warning (default: 4311 4302)

        PIMAGE_BASE_RELOCATION current_reloc_block = reinterpret_cast<PIMAGE_BASE_RELOCATION>(base_u8 + nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
        while (current_reloc_block && current_reloc_block->VirtualAddress)
        {
            std::uint16_t *relocs = reinterpret_cast<std::uint16_t *>(current_reloc_block + 1);
            std::size_t    relocs_count = (current_reloc_block->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(std::uint16_t);
            for (int i = 0; i < relocs_count; i++)
                if ((relocs[i] & 0xF000) == 0x3000 /*IMAGE_REL_BASED_HIGHLOW*/)
                    *reinterpret_cast<std::uint32_t *>(base_u8 + (current_reloc_block->VirtualAddress + (relocs[i] & 0x0FFF))) += delta;

            current_reloc_block = utils::base_relocation_block_forward(current_reloc_block);
        }
    }

    _WINTERNAL_INLINE bool unmapped_pe_load_iat(void *base, decltype(LoadLibraryA) *winapi_LoadLibraryA, decltype(GetProcAddress) *winapi_GetProcAddress)
    {
        std::uint8_t *base_u8 = reinterpret_cast<std::uint8_t *>(base);
        PIMAGE_NT_HEADERS        nt_header = utils::pe_get_ntheaderptr(base);
        PIMAGE_IMPORT_DESCRIPTOR current_import_desc = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(base_u8 + nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
        while (current_import_desc->Characteristics)
        {
            PIMAGE_THUNK_DATA oft = reinterpret_cast<PIMAGE_THUNK_DATA>(base_u8 + current_import_desc->OriginalFirstThunk);
            PIMAGE_THUNK_DATA ft = reinterpret_cast<PIMAGE_THUNK_DATA>(base_u8 + current_import_desc->FirstThunk);
            HMODULE mod = winapi_LoadLibraryA(reinterpret_cast<const char *>(base_u8 + current_import_desc->Name));

            if (!mod)
                return false;

            while (oft->u1.AddressOfData)
            {
                std::uintptr_t fn = reinterpret_cast<std::uintptr_t>(winapi_GetProcAddress(mod,
                    oft->u1.Ordinal & IMAGE_ORDINAL_FLAG ? reinterpret_cast<const char *>(oft->u1.Ordinal & 0xFFFF) : reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(base_u8 + oft->u1.AddressOfData)->Name
                    ));

                if (!fn)
                    return false;

                ft->u1.Function = fn;

                ++oft;
                ++ft;
            }

            ++current_import_desc;
        }

        return true;
    }

    _WINTERNAL_INLINE std::size_t unsafe_get_subroutine_size(void *entry)
    {
        std::uint8_t *entry_u8 = reinterpret_cast<std::uint8_t *>(entry);
        std::size_t   subroutine_size = 0;

        while (entry_u8[subroutine_size++] != 0xCC);

        return subroutine_size - 1;
    }

    std::unique_ptr<std::uint8_t[]> load_raw_pe_sections_to_local_memory(void *pe_bin);
}