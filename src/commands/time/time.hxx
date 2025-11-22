#pragma once

#include "commands/cmd.hxx"

#include <functional>
#include <chrono>

namespace pnd::cmd {

	using TimePoint = std::chrono::utc_clock::time_point;
	using NowFn = std::function<TimePoint()>;

	struct time : public Cmd
	{
		static constexpr const std::string name = "time";
		time(NowFn now_fn = utc_now);

		static TimePoint utc_now();
	};

} //namespace pnd::cmd
