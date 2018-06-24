#pragma once
#include <condition_variable>
#include <map>

namespace details
{
	class t_delegate;
}

class timer
{
	
public:

	using timer_attachment_id = std::uint64_t;
	static timer_attachment_id constexpr invalid_id = timer_attachment_id(0);
	
	using handler_type = std::function<void()>;
	using duration = std::chrono::milliseconds;

	/* Default period is 1000
	 * early_invoking indicates whether to execute handlers as soon as a call to enable has been called.
	 * If true handlers will be invoked at once. If false they will be invoked after period
	 */
	explicit timer(duration ms_period = duration(1000), bool early_invoking = true);
	~timer();

	// Remove copy and move assignment/construction
	timer(timer&&) = delete;
	timer& operator=(timer&&) = delete;
	timer(timer const&) = delete;
	timer& operator=(timer const&) = delete;

	timer_attachment_id attach_handler(handler_type handler);
	bool detach_handler(timer_attachment_id id);

	void clear();
	std::size_t size() const noexcept;
	bool empty() const noexcept;

	void set_period(duration ms_period);
	duration get_period() const noexcept;

	bool get_early_invoking() const noexcept;
	void set_early_invoking(bool val);

	void enable();
	void disable();

private:

	using condition_var = std::condition_variable;
	using clock = std::chrono::steady_clock;
	using timestamp = std::chrono::time_point<clock>;
	using lock = std::mutex;
	using scoped_lock = std::unique_lock<lock>;
	using delegate_map = std::map<timer_attachment_id, details::t_delegate>;

	bool remove_timer_delegate(scoped_lock& lock, delegate_map::iterator& iter);

	timer_attachment_id next_id_;
	delegate_map delegates_;

	duration period_;
	duration first_shot_;
	timestamp next_execution_point_;

	bool running_; // running is purely for internal use
	bool enabled_;

	mutable lock lock_;
	condition_var signaler_;
	std::thread worker_thread_;
	void timer_thread_worker();

};


