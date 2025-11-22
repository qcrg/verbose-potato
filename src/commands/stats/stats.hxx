#pragma once

#include "commands/cmd.hxx"
#include "connection_stats.hxx"
#include <functional>
#include <string>

namespace pnd::cmd {

	using GetStatsFn = std::function<ConnectionStats()>;

	struct stats : public Cmd
	{
		static constexpr const std::string name = "stats";
		stats(GetStatsFn get_stats);
	};

} // namespace pnd::cmd
