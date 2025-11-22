#include "shutdown.hxx"

#include "commands/cmd.hxx"
#include "jlog.hxx"

namespace pnd::cmd {

	ProcessCmdFn mkfn(ExitFn fn) {
		return [fn](buf_t, ProcessArgs) -> Exp<buf_t> {
			jlog::info("Shutting down...");
			fn(0);
			return {};
		};
	}

	shutdown::shutdown(ExitFn fn)
		: Cmd{shutdown::name, mkfn(fn)}
	{}

} //namespace pnd::cmd
