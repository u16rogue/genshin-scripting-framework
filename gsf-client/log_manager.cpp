#include "log_manager.h"

static std::deque<gsf::log_manager::log_cont> logs;

const std::deque<gsf::log_manager::log_cont> &gsf::log_manager::get_logs()
{
	return logs;
}

void gsf::log_manager::push_log(std::string txt, log_type_t type)
{
	logs.emplace_back(std::move(txt), type);
}

void gsf::log_manager::clear_log()
{
	logs.clear();
}
