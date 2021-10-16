#include "script_manager.h"

#include <Windows.h>
#include <imgui.h>
#include <cstddef>
#include <cstdint>
#include <macro.h>

static std::vector<gsf::script> script_instances;

const std::vector<gsf::script> &gsf::script_manager::get_scripts()
{
    return script_instances;
}

bool gsf::script_manager::script_autoexec(std::string_view file_path)
{
    if (!std::filesystem::exists(file_path))
        return false;

    sol::state autoexec_script;

    auto namespace_gsf = autoexec_script["gsf"].get_or_create<sol::table>();
    namespace_gsf.set_function("import", [](const char *file_path) -> bool
    {
        return gsf::script_manager::script_import(file_path);
    });

    namespace_gsf.set_function("load", [](const char *file_path) -> bool
    {
        if (gsf::script *script = nullptr; gsf::script_manager::script_import(file_path, &script))
            return script->load();

        return false;
    });

    autoexec_script.script_file(file_path.data());

    return true;
}

bool gsf::script_manager::script_import(std::string_view file_path, gsf::script **script_instance_out)
{
    if (!std::filesystem::exists(file_path))
        return false;

    for (auto &script : script_instances)
        if (script.get_filepath() == file_path)
            return false;

    auto &loaded_script = script_instances.emplace_back(file_path);
    if (script_instance_out)
        *script_instance_out = &loaded_script;

    return true;
}

void gsf::script_manager::unload_all_scripts()
{
    for (auto &script : script_instances)
        script.unload();
}
