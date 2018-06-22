#ifndef TIMER_INFO_H
#define TIMER_INFO_H

#include <cstdint>
#include <functional>
#include <condition_variable>

namespace details
{
	class timer_info
	{
		
		friend class timer_manager;

		using timer_id = std::uint64_t;
		using handler_type = std::function<void()>;
		using condition_var = std::condition_variable;

		using clock = std::chrono::steady_clock;
		using time_stamp = std::chrono::time_point<clock>;
		using duration = std::chrono::milliseconds;
		
	public:

		struct timer_info_comparator
		{
			bool operator()(timer_info const& right, timer_info const& left) const noexcept;
		};

		~timer_info();

	private:

		timer_info(timer_id id,
			time_stamp next_timeout,
			duration period,
			handler_type target_handler) noexcept;
		explicit timer_info(timer_id id = 0);
		timer_info(timer_info&& other) noexcept;
		timer_info& operator=(timer_info&& other) noexcept;

		timer_info(timer_info const&) = delete;
		timer_info& operator=(timer_info const&) = delete;

		timer_id id_;
		time_stamp first_timeout_;
		duration period_;
		handler_type target_handler_;

		bool running_;
		std::unique_ptr<condition_var> wait_conditional_;

	};
}

#endif



