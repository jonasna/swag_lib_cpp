#include "timer.h"

timer::timer(int ms_period) : period_(ms_period), running_(false), next_id_(invalid_id + 1)
{

}

timer::~timer()
{
}

timer::timer_attachment_id timer::attach_handler(handler_type handler)
{
	scoped_lock lock(lock_);

	auto id = next_id_++;
	delegates_.emplace_back(timer_delegate(id, std::move(handler)));

	return timer_attachment_id();
}

void timer::timer_thread_worker()
{
	scoped_lock lock(lock_);

	while(running_)
	{
		signaler_.wait_for(lock, period_);

		for (auto i = 0; i < delegates_.size(); ++i)
		{

			auto& delegate = delegates_[i];
			delegate.active_ = true;

			// Release and invoke
			lock.unlock();
			delegate.target_handler_();
			lock.lock();

			if(delegate.active_)
			{
				
			}else
			{
				
			}

		}
	}
}

timer::timer_delegate::timer_delegate(timer_attachment_id id) : info_id_(id), active_(false)
{
}

timer::timer_delegate::timer_delegate(timer_delegate && other) noexcept
	: info_id_(std::move(other.info_id_)), target_handler_(std::move(other.target_handler_)), active_(std::move(other.active_))
{
}

timer::timer_delegate& timer::timer_delegate::operator=(timer_delegate && other) noexcept
{
	if(this != &other)
	{
		info_id_ = std::move(other.info_id_);
		target_handler_ = std::move(other.target_handler_);
		active_ = std::move(other.active_);
	}

	return *this;
}

timer::timer_delegate::timer_delegate(timer_attachment_id id, handler_type handler) noexcept
	: info_id_(id), target_handler_(std::move(handler)), active_(false)
{
}

bool timer::timer_delegate::operator==(const timer_delegate & other) const
{
	return info_id_ == other.info_id_;
}
