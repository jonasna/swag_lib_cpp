#pragma once
#include <condition_variable>
#include <vector>

class timer
{

	using condition_var = std::condition_variable;
	using clock = std::chrono::steady_clock;
	using time_stamp = std::chrono::time_point<clock>;
	using duration = std::chrono::milliseconds;
	using lock = std::mutex;
	using scoped_lock = std::unique_lock<lock>;

public:

	using timer_attachment_id = std::uint64_t;
	using handler_type = std::function<void()>;

	static timer_attachment_id constexpr invalid_id = timer_attachment_id(0);

	explicit timer(int ms_period);
	~timer();

	timer_attachment_id attach_handler(handler_type handler);
	bool detach_handler(timer_attachment_id id);

	void set_period(int ms_period);
	int get_period() const;

	void start();
	void stop();

	bool enable();
	bool disable();

private:

	struct timer_delegate
	{
		explicit timer_delegate(timer_attachment_id id = 0);
		timer_delegate(timer_delegate&& other) noexcept;
		timer_delegate& operator=(timer_delegate&& other) noexcept;
		timer_delegate(timer_attachment_id id, handler_type handler) noexcept;

		timer_attachment_id info_id_;
		handler_type target_handler_;
		bool operator==(const timer_delegate& other) const;
		bool active_;
		// For synchronization on removal
		std::unique_ptr<condition_var> wait_conditional;
	};

	timer_attachment_id next_id_;
	std::vector<timer_delegate> delegates_;
	duration period_;
	bool running_;

	mutable lock lock_;
	condition_var signaler_;
	std::thread worker_;
	void timer_thread_worker();

};

