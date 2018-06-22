#pragma once

#include "timer_info.h"
#include <set>

class timer_manager
{
	using lock = std::mutex;
	using scoped_lock = std::unique_lock<lock>;
	using timer_info = details::timer_info;

	using timer_info_ref = std::reference_wrapper<timer_info>;
	using reference_queue = std::multiset<timer_info_ref, details::timer_info::timer_info_comparator>;


public:
	timer_manager();
	~timer_manager();

private:

};

