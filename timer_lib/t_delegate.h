#pragma once
#include <functional>
#include <memory>
#include <condition_variable>

namespace details
{
	class t_delegate
	{
	public:

		using timer_attachment_id = std::uint64_t;
		using handler_type = std::function<void()>;

		explicit t_delegate(timer_attachment_id id = 0);
		t_delegate(timer_attachment_id id, handler_type handler) noexcept;
		t_delegate(t_delegate&& other) noexcept;
		t_delegate& operator=(t_delegate&& other) noexcept;

		~t_delegate() = default;

		t_delegate(t_delegate const&) = delete;
		t_delegate& operator=(t_delegate const&) = delete;

		timer_attachment_id get_id() const;
		void execute() const;
		bool get_is_executing() const;
		void set_is_executing(bool executing);

		std::unique_ptr<std::condition_variable>& get_removal_conditional() const;

	private:

		timer_attachment_id attachment_id_;
		handler_type handler_;
		bool executing_;

		// For synchronization on removal
		mutable std::unique_ptr<std::condition_variable> removal_conditional_;

	};

}


