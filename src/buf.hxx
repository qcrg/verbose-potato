#pragma once

#include "cast.hxx"

#include <span>
#include <string_view>

namespace pnd {
	using buf_t = std::span<std::byte>;

	inline constexpr std::string_view make_sview(buf_t buf) {
		return std::string_view(rcast<char*>(buf.data()), buf.size());
	}

} //namespace pnd
