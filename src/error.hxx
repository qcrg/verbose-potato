#pragma once

#include <expected>
#include <format>
#include <string>
#include <system_error>
#include <type_traits>
#include <sstream>

namespace pnd {

	namespace impl {
		template<typename T, typename = void>
		struct Exp
		{
			using type = std::expected<T, std::string>;
		};

		template<typename T>
		struct Exp<T, std::enable_if_t<std::is_lvalue_reference_v<T>>>
		{
			using type = std::expected<
				std::reference_wrapper<std::remove_reference_t<T>>,
				std::string
			>;
		};

		template<typename T>
		using ExpT = Exp<T>::type;
	} //namespace impl

	template<typename T>
	concept ErrorConstructTypes =
		std::is_convertible_v<T, std::string_view> ||
		std::is_integral_v<T>;

	template<typename T>
	using Exp = impl::ExpT<T>;

	struct Error : public std::unexpected<std::string>
	{
		using unexp_t = std::unexpected<std::string>;

		constexpr Error(std::string err) : unexp_t{err} {}

		template<typename ...Args>
		constexpr Error(std::format_string<Args...> fmt, Args&&... args)
			: unexp_t{std::format(fmt, std::forward<Args>(args)...)}
		{}

		constexpr Error(
			int err,
			const std::error_category& category = std::generic_category()
		) : unexp_t{errno_to_string(err, category)}
		{}

		constexpr Error(Error& other) = default;
		constexpr Error(const Error& other) = default;
		constexpr Error(Error&& other) = default;

		template<ErrorConstructTypes ...Args>
		constexpr static Error joind(const char* delemiter, Args ...args) {
			std::ostringstream out;
			bool first = true;
			(void)first;
			(
				(
					out << (first ? "" : delemiter) << Error(args).error(),
					first = false
				),
				...
			);
			return Error(out.str());
		}

		template<ErrorConstructTypes ...Args>
		constexpr static Error join(Args ...args) {
			return joind(": ", args...);
		}

		constexpr static std::string errno_to_string(
			int err,
			const std::error_category& category = std::generic_category()
		) {
			return category.message(err);
		}

	};

} //namespace pnd
