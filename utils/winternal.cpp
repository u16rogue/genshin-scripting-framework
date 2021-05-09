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
    static utils::ldr_data_table_entry *first_entry = [] () -> auto
    {
        #ifdef _M_IX86
            return reinterpret_cast<peb *>(__readfsdword(0x30))->ldr->entry;
        #elif _M_X64
            return reinterpret_cast<peb *>(__readgsqword(0x60))->ldr->entry;
        #endif
    }();

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