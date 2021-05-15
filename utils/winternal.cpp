#include "winternal.h"

std::unique_ptr<std::uint8_t[]> utils::load_raw_pe_sections_to_local_memory(void *pe_bin)
{
    if (!utils::pe_validate_dosheader(pe_bin))
        return nullptr;

    auto nt_header = utils::pe_get_ntheaderptr(pe_bin);
    if (!nt_header)
        return nullptr;

    std::unique_ptr<std::uint8_t[]> mapped_pe_sections = std::make_unique<std::uint8_t[]>(nt_header->OptionalHeader.SizeOfImage);

    memcpy(mapped_pe_sections.get(), pe_bin, nt_header->OptionalHeader.SizeOfHeaders);

    PIMAGE_SECTION_HEADER sections = reinterpret_cast<PIMAGE_SECTION_HEADER>(nt_header + 1);
    for (int i = 0; i < nt_header->FileHeader.NumberOfSections; i++)
        memcpy(mapped_pe_sections.get() + sections[i].VirtualAddress, reinterpret_cast<std::uint8_t*>(pe_bin) + sections[i].PointerToRawData, sections[i].SizeOfRawData);

    return mapped_pe_sections;
}
