#include "time.hxx"

#include "cast.hxx"

#include <chrono>
#include <format>

namespace pnd::cmd {

	TimePoint time::utc_now() {
		return std::chrono::utc_clock::now();
	}

	Exp<buf_t> process_time(
		NowFn now,
		buf_t out_buf,
		std::span<const std::string_view>
	) {
		const auto res = std::format_to_n(
			rcast<char*>(out_buf.data()),
			out_buf.size(),
			"{:%Y-%m-%d %H:%M:%S}\n",
			std::chrono::floor<std::chrono::seconds>(now())
		);
		return out_buf.subspan(0, res.size);
	}

	ProcessCmdFn mk_call(NowFn now_fn) {
		return [=](buf_t out_buf, std::span<const std::string_view> args) {
			return process_time(now_fn, out_buf, args);
		};
	}

	time::time(NowFn now_fn)
		: Cmd{time::name, mk_call(now_fn)}
	{}

} //namespace pnd::cmd
