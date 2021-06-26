#include "cs_script.h"

gsf::cs_script::cs_script(const std::wstring_view filepath_)
	: filepath(filepath_)
{
	gsf::cs_script::instances.push_back(this);
}

bool gsf::cs_script::load()
{
	return false;
}

bool gsf::cs_script::unload()
{
	return false;
}

const int gsf::cs_script::get_id()
{
	return this->id;
}

bool gsf::cs_script::factory::add(std::wstring_view filepath_entry)
{
	return false;
}

bool gsf::cs_script::factory::remove(const int id)
{
	return false;
}
