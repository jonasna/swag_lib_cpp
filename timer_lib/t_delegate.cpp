#include "t_delegate.h"

namespace details
{
	t_delegate::t_delegate(timer_attachment_id id) : attachment_id_(id), executing_(false)
	{
	}

	t_delegate::t_delegate(timer_attachment_id id, handler_type handler) noexcept
		: attachment_id_(id), handler_(std::move(handler)), executing_(false)
	{
	}

	t_delegate::t_delegate(t_delegate && other) noexcept
		: attachment_id_(other.attachment_id_), handler_(std::move(other.handler_)), executing_(other.executing_)
	{
	}

	t_delegate & t_delegate::operator=(t_delegate && other) noexcept
	{
		if(this != &other)
		{
			attachment_id_ = other.attachment_id_;
			handler_ = std::move(other.handler_);
			executing_ = other.executing_;
		}
		return *this;
	}

	t_delegate::timer_attachment_id t_delegate::get_id() const
	{
		return attachment_id_;
	}

	void t_delegate::execute() const
	{
		handler_();
	}

	bool t_delegate::get_is_executing() const
	{
		return executing_;
	}

	void t_delegate::set_is_executing(bool executing)
	{
		executing_ = executing;
	}

	std::unique_ptr<std::condition_variable>& t_delegate::get_removal_conditional() const
	{
		return removal_conditional_;
	}
}


