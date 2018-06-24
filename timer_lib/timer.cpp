#include "timer.h"
#include "t_delegate.h"
#include <algorithm>

using namespace details;

timer::timer(duration ms_period, bool early_invoking)
	: next_id_(invalid_id + 1), period_(ms_period), first_shot_(early_invoking ? duration(0) : ms_period), running_(false), enabled_(false)
{

}

timer::~timer()
{
	scoped_lock lock(lock_);

	if (worker_thread_.joinable())
	{
		running_ = false; // Set running to false
		lock.unlock();

		signaler_.notify_all(); // Signal thread
		worker_thread_.join(); // Wait for it to finish
	}
}

timer::timer_attachment_id timer::attach_handler(handler_type handler)
{
	scoped_lock lock(lock_);

	auto id = next_id_++;
	delegates_.emplace(id, t_delegate(id, handler));
		
	return id;
}

bool timer::detach_handler(timer_attachment_id id)
{
	scoped_lock lock(lock_);
	auto target = delegates_.find(id);
	return remove_timer_delegate(lock, target);
}

void timer::clear()
{
	scoped_lock lock(lock_);
	for(auto iter = delegates_.begin(); iter != delegates_.end();)
	{
		remove_timer_delegate(lock, iter);
		iter = delegates_.begin();
	}
}

std::size_t timer::size() const noexcept
{
	scoped_lock lock(lock_);
	return delegates_.size();
}

bool timer::empty() const noexcept
{
	scoped_lock lock(lock_);
	return delegates_.empty();
}

void timer::set_period(duration ms_period)
{
	scoped_lock lock(lock_);
	period_ = ms_period;
}

timer::duration timer::get_period() const noexcept
{
	scoped_lock lock(lock_);
	return period_;
}

bool timer::get_early_invoking() const noexcept
{
	scoped_lock lock(lock_);
	return first_shot_.count() == 0;
}

void timer::set_early_invoking(bool val)
{
	scoped_lock lock(lock_);
	first_shot_ = val ? duration(0) : period_;
}

void timer::enable()
{
	scoped_lock lock(lock_);

	if (!worker_thread_.joinable()) // Lazy initiation of the of the worker thread
	{
		running_ = true;
		worker_thread_ = std::thread(&timer::timer_thread_worker, this);
	}
	
	if(!enabled_)
	{
		next_execution_point_ = clock::now() + first_shot_;
		enabled_ = true;
		signaler_.notify_all();
	}
}

void timer::disable()
{
	scoped_lock lock(lock_);

	if(enabled_)
	{
		enabled_ = false;
		signaler_.notify_all();
		if (std::this_thread::get_id() != worker_thread_.get_id()) // To avoid dead locking
		{
			signaler_.wait(lock); // Wait for worker to acknowledge
		}
	}
}

bool timer::remove_timer_delegate(scoped_lock& lock, delegate_map::iterator& iter)
{
	if (iter == delegates_.end())
		return false;

	auto& target_delegate = iter->second;

	if(target_delegate.get_is_executing())
	{
		// The delegate is being invoked

		// Request removal
		target_delegate.set_is_executing(false);

		// Set and wait for handler to finish execution
		// Handler will remove the delegate
		target_delegate.get_removal_conditional().reset(new condition_var);
		if(std::this_thread::get_id() != worker_thread_.get_id()) // To avoid dead locking
		{
			target_delegate.get_removal_conditional()->wait(lock);
		}

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
		signaler_.wait_until(lock, next_execution_point_);

		for (auto iter = delegates_.begin(); iter != delegates_.end(); /* Iterator is incremented in the loop */)
																	   // Standard associative-container erase idiom 
		{
			if (!running_ || !enabled_) // Check if we were woken up due to being disabled
				break;

			auto& target_delegate = iter->second;

			target_delegate.set_is_executing(true);

			// Release and invoke
			lock.unlock();
			target_delegate.execute();
			lock.lock();

			if (!target_delegate.get_is_executing())
			{
				// Someone requested the removal of the executing delegate...

				// The thread who requested the removal is waiting for
				// a signal to continue
				target_delegate.get_removal_conditional()->notify_all();

				// Also it expects us to remove the delegate
				iter = delegates_.erase(iter); // and set iterator
			}
			else
			{
				target_delegate.set_is_executing(false);
				++iter;
			}
		}

		if (!enabled_) // If timer is not enabled or was disabled
		{
			signaler_.notify_all(); // Disabling thread is waiting for notification that this point was reached				
			signaler_.wait(lock, [this] {
				return (!running_ || enabled_);
			});
			
		}else
		{
			next_execution_point_ = clock::now() + period_;
		}
					
	}
}



