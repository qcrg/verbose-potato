#pragma once

#include "commands/cmd.hxx"

#include <cstdlib>
#include <functional>

namespace pnd::cmd {

	// exit with status
	using ExitFn = std::function<void(int)>;

	struct shutdown : public Cmd
	{
		static constexpr const std::string name = "shutdown";
		shutdown(ExitFn exit = std::exit);
	};

} // namespace pnd::cmd
