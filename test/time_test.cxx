#include "commands/time/time.hxx"

#include "cast.hxx"

#include "doctest.h"
#include <chrono>
#include <string_view>

using namespace pnd;

const size_t TIMESTAMP = 1763631255;
const std::string_view TIMESTAMP_STR = "2025-11-20 09:34:15\n";

using cmd::TimePoint;

TimePoint mock_now() {
	return TimePoint(std::chrono::utc_clock::from_sys(
		std::chrono::sys_seconds(std::chrono::seconds(TIMESTAMP)))
	);
}

TEST_CASE("time command") {
	std::byte buf[32];

	cmd::time inst = cmd::time{mock_now};
	Exp<buf_t> exp = inst(buf, {});
	REQUIRE(exp.has_value());
	buf_t val = exp.value();
	REQUIRE(buf == val.data());
	auto val_str = std::string_view(rcast<char*>(val.data()), val.size());
	CHECK(val_str == TIMESTAMP_STR);
}
