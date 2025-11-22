#pragma once

#include "error.hxx"
#include "buf.hxx"

#include <functional>
#include <span>
#include <string_view>

namespace pnd {

	using ProcessArgs = std::span<const std::string_view>;
	using ProcessCmdFn = std::function<Exp<buf_t>(buf_t out_buf, ProcessArgs)>;

	struct Cmd
	{
		std::string name;
		ProcessCmdFn call;

		Cmd(std::string name, ProcessCmdFn call)
			: name{std::move(name)}
			, call{std::move(call)}
		{}

		inline Exp<buf_t> operator()(
			buf_t out_buf,
			std::span<const std::string_view> args
		) const {
			return call(out_buf, args);
		}
	};

} //namespace pnd
