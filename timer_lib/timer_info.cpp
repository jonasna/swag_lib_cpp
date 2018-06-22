#include "timer_info.h"

namespace details
{

	timer_info::~timer_info()
	{
	}

	timer_info::timer_info(timer_id id,
						   time_stamp next_timeout,
					       duration period,
		                   handler_type target_handler) noexcept
		: id_(id)
		, first_timeout_(next_timeout)
		, period_(period)
		, target_handler_(std::move(target_handler))
		, running_(false)
	{
	}

	timer_info::timer_info(timer_id id) : id_(id), running_(false)
	{

	}

	timer_info::timer_info(timer_info&& other) noexcept
		: id_(std::move(other.id_))
		, first_timeout_(std::move(other.first_timeout_))
		, period_(std::move(other.period_))
		, target_handler_(std::move(other.target_handler_))
		, running_(std::move(other.running_))
	{
	}

	timer_info& timer_info::operator=(timer_info && other) noexcept
	{
		if(this != &other)
		{
			id_ = std::move(other.id_);
			first_timeout_ = std::move(other.first_timeout_);
			period_ = std::move(other.period_);
			target_handler_ = std::move(other.target_handler_);
			running_ = std::move(other.running_);
		}
		return *this;
	}

	bool timer_info::timer_info_comparator::operator()(timer_info const & right, timer_info const & left) const noexcept
	{
		return right.first_timeout_ < left.first_timeout_;
	}
}


