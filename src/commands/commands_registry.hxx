#pragma once

#include "cmd.hxx"

#include <flat_map>
#include <string>

namespace pnd {

	class CommandsRegistry
	{
		std::flat_map<std::string, Cmd> _data;
	public:
		explicit CommandsRegistry() = default;

		Exp<void> insert(Cmd cmd);
		Exp<const Cmd&> get(const std::string& name) const;
		bool has(const std::string& name) const;
	};

} //namespace pnd
