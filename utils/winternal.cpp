#include "winternal.h"
#include <intrin.h>

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
    std::uint8_t   pad0[4];
    std::uintptr_t pad1[2];
public:
    peb_ldr_data *ldr;
};

bool utils::ldr_data_table_entry_next(utils::ldr_data_table_entry *&dest)
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

bool utils::pe_validate_dosheader(void *base)
{
    return reinterpret_cast<PIMAGE_DOS_HEADER>(base)->e_magic == IMAGE_DOS_SIGNATURE;
}

PIMAGE_DOS_HEADER utils::pe_get_dosheaderptr(void *base)
{
    return reinterpret_cast<PIMAGE_DOS_HEADER>(base);
}

PIMAGE_NT_HEADERS utils::pe_get_ntheaderptr(void *base)
{
    return reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uintptr_t>(base) + utils::pe_get_dosheaderptr(base)->e_lfanew);
}

utils::ldr_data_table_entry *utils::ldr_data_table_entry_find(const wchar_t *name)
{
    utils::ldr_data_table_entry *entry = nullptr;
    while (utils::ldr_data_table_entry_next(entry))
    {
        if (!entry->dll_base)
            continue;

        if (wcscmp(entry->base_dll_name, name) == 0)
            return entry;
    }

    return nullptr;
}

bool utils::ldr_data_table_entry_find(const wchar_t *name, utils::ldr_data_table_entry *&dest)
{
    dest = utils::ldr_data_table_entry_find(name);

    if (!dest)
        return false;

    return true;
}

void *utils::export_fn_get(void *mod_base, const char *name)
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
        if (strcmp(reinterpret_cast<const char *>(base + names[i]), name) == 0)
            return base + (address[ordinals[i]]);

    return nullptr;
}

void *utils::export_fn_find(const char *name)
{
    utils::ldr_data_table_entry *entry = nullptr;

    while (utils::ldr_data_table_entry_next(entry))
    {
        if (!entry->dll_base)
            continue;
        
        if (void *result = utils::export_fn_get(entry->dll_base, name); result)
            return result;
    }

    return nullptr;
}

PIMAGE_BASE_RELOCATION utils::base_relocation_block_forward(PIMAGE_BASE_RELOCATION current_block)
{
    return reinterpret_cast<PIMAGE_BASE_RELOCATION>(reinterpret_cast<std::uintptr_t>(current_block) + current_block->SizeOfBlock);
}
