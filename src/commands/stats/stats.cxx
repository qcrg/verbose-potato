#include "stats.hxx"

#include "commands/cmd.hxx"

namespace pnd::cmd {

	ProcessCmdFn mkfn(GetStatsFn fn) {
		return [fn](buf_t out_buf, ProcessArgs) -> Exp<buf_t> {
			ConnectionStats data = fn();
			const auto res = std::format_to_n(
				rcast<char*>(out_buf.data()),
				out_buf.size(),
				"Connected: {}; Total connections: {}\n",
				data.current_connected,
				data.total_connections
			);
			return out_buf.subspan(0, res.size);
		};
	}

	stats::stats(GetStatsFn fn)
		: Cmd{stats::name, mkfn(fn)}
	{}

} //namespace pnd::cmd
