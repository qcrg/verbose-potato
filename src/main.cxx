#include "service/service.hxx"
#include "commands/commands_registry.hxx"
#include "commands/time/time.hxx"
#include "commands/shutdown/shutdown.hxx"
#include "commands/stats/stats.hxx"

#include "jlog.hxx"
#include <functional>

using namespace pnd;
using Srv = service::Service;

Exp<CommandsRegistry> make_registry(const Srv& srv)
{
	CommandsRegistry reg;

	Exp<void> ok;

	// time
	{
		ok = reg.insert(cmd::time{});
		if (!ok)
			return Error(ok.error());
	}
	// shutdown
	{
		ok = reg.insert(cmd::shutdown{});
		if (!ok)
			return Error(ok.error());
	}
	// stats
	{
		ok = reg.insert(cmd::stats{std::bind(&Srv::get_connections_stat, &srv)});
		if (!ok)
			return Error(ok.error());
	}

	return reg;
}

int main()
{
	Exp<Srv> srv = Srv::make(ipv4::localhost, 8888);
	if (!srv) {
		jlog::error("Failed to initialize Service: {}", srv.error());
		return 1;
	}

	auto reg = make_registry(srv.value());
	if (!reg) {
		jlog::error("Failed to initialize CommandsRegistry: {}", reg.error());
		return 1;
	}

	auto ok = srv->run(reg.value());
	if (!ok) {
		jlog::error("Failed to run Service: {}", ok.error());
		return 1;
	}
	return 0;
}
