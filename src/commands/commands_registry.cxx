#include "commands_registry.hxx"

#include "jlog.hxx"
#include <format>

namespace pnd {

	Exp<void> CommandsRegistry::insert(Cmd cmd) {
		auto [it, is_inserted] = _data.insert({cmd.name, std::move(cmd)});
		if (is_inserted)
			jlog::debug("Command '{}' added", it->first);
		else
			return Error("Command '{}' is already added", it->first);
		return {};
	}

	Exp<const Cmd&> CommandsRegistry::get(const std::string& name) const {
		auto it = _data.find(name);
		if (it == _data.end())
			return Error("Command '{}' is not registered", name);
		return it->second;
	}

	bool CommandsRegistry::has(const std::string& name) const {
		return _data.find(name) != _data.end();
	}

} //namespace pnd
