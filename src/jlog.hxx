#pragma once

#include <chrono>
#include <format>
#include <iostream>
#include <utility>

namespace pnd {


	namespace jlog {

		enum class Level
		{
			fatal = 0,
			error,
			warn,
			info,
			debug,
			trace
		};

		constexpr const char* _level_to_str(Level level) {
			static constexpr const char* names[] = {
				"\x1b[41;97mfatal\x1b[0m",
				"\x1b[31;1merror\x1b[0m",
				"\x1b[33;1mwarn\x1b[0m",
				"\x1b[32minfo\x1b[0m",
				"\x1b[36mdebug\x1b[0m",
				"\x1b[90mtrace\x1b[0m",
			};
			return names[static_cast<int>(level)];
		}

		inline Level global_level = Level::info;

		template<typename ...Args>
		void _log(Level level, std::format_string<Args...> fmt, Args&&... args) {
			if (level > global_level)
				return;
			namespace sc = std::chrono;
			std::clog
				<< std::format(
					"[{:%Y-%m-%d %H:%M:%S}] [{}] ",
					sc::utc_clock::now(),
					_level_to_str(level)
				)
				<< std::format(fmt, std::forward<Args>(args)...)
				<< '\n';
		}

		template<typename ...Args>
		void fatal(std::format_string<Args...> fmt, Args&&... args) {
			_log(Level::fatal, fmt, std::forward<Args>(args)...);
			std::terminate();
		}

		template<typename ...Args>
		void error(std::format_string<Args...> fmt, Args&&... args) {
			return _log(Level::error, fmt, std::forward<Args>(args)...);
		}

		template<typename ...Args>
		void warn(std::format_string<Args...> fmt, Args&&... args) {
			return _log(Level::warn, fmt, std::forward<Args>(args)...);
		}

		template<typename ...Args>
		void info(std::format_string<Args...> fmt, Args&&... args) {
			return _log(Level::info, fmt, std::forward<Args>(args)...);
		}

		template<typename ...Args>
		void debug(std::format_string<Args...> fmt, Args&&... args) {
			return _log(Level::debug, fmt, std::forward<Args>(args)...);
		}

		template<typename ...Args>
		void trace(std::format_string<Args...> fmt, Args&&... args) {
			return _log(Level::trace, fmt, std::forward<Args>(args)...);
		}

	} //namespace log
} //namespace pnd
