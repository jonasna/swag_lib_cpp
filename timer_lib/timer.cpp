#include "timer.h"

timer::timer(int ms_period) : next_id_(invalid_id + 1), period_(ms_period), running_(false)
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

	return id;
}

bool timer::detach_handler(timer_attachment_id id)
{
	scoped_lock lock(lock_);
	auto target = std::find(delegates_.begin(), delegates_.end(), timer_delegate(id));
	return remove_timer_delegate(lock, target);
}

bool timer::remove_timer_delegate(scoped_lock& lock, std::vector<timer_delegate>::iterator& iter)
{
	if (iter == delegates_.end())
		return false;

	auto& target_delegate = *iter;

	if(target_delegate.active_)
	{
		// The delegate is being invoked

		// Request removal
		target_delegate.active_ = false;

		// Set and wait for handler to finish execution
		// Handler will remove the delegate
		target_delegate.wait_conditional.reset(new condition_var);
		target_delegate.wait_conditional->wait(lock);
	}else
	{
		delegates_.erase(iter);
	}

	return true;
}

void timer::timer_thread_worker()
{
	scoped_lock lock(lock_);

	while(running_)
	{
		signaler_.wait_for(lock, period_);

		for (auto& delegate : delegates_)
		{
			delegate.active_ = true;

			// Release and invoke
			lock.unlock();
			delegate.target_handler_();
			lock.lock();

			if(delegate.active_)
			{
				
			}else
			{
				// Someone requested the removal of the delegate...

				// The thread who requested the removal is waiting for
				// a signal
				delegate.wait_conditional->notify_all();
				
			}

		}
	}
}


/*
 *	Below section describes the implementation of timer_delegate
 */

timer::timer_delegate::timer_delegate(timer_attachment_id id) : info_id_(id), active_(false)
{
}

timer::timer_delegate::timer_delegate(timer_delegate && other) noexcept
	: info_id_(other.info_id_), target_handler_(std::move(other.target_handler_)), active_(other.active_)
{
}

timer::timer_delegate& timer::timer_delegate::operator=(timer_delegate && other) noexcept
{
	if(this != &other)
	{
		info_id_ = other.info_id_;
		target_handler_ = std::move(other.target_handler_);
		active_ = other.active_;
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
