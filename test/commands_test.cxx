#include "commands/commands_registry.hxx"
#include "commands/time/time.hxx"
#include "commands/shutdown/shutdown.hxx"
#include "commands/stats/stats.hxx"
#include "connection_stats.hxx"
#include "doctest.h"

using namespace pnd;

TEST_CASE("init commands") {
	auto reg = CommandsRegistry{};
	Exp<void> ok;

	ok = reg.insert(cmd::time{});
	REQUIRE_MESSAGE(ok, ok.error());
	CHECK(reg.has(cmd::time::name));

	ok = reg.insert(cmd::shutdown{});
	REQUIRE_MESSAGE(ok, ok.error());
	CHECK(reg.has(cmd::shutdown::name));

	cmd::GetStatsFn get_stats_mock = []() -> ConnectionStats { return {}; };
	ok = reg.insert(cmd::stats{get_stats_mock});
	REQUIRE_MESSAGE(ok, ok.error());
	CHECK(reg.has(cmd::stats::name));
}
