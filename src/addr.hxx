#pragma once

#include "cast.hxx"

#include <bit>
#include <cstdint>
#include <netinet/in.h>
#include <format>

namespace pnd {

	namespace _ipv4_utils {
		constexpr uint32_t left(uint8_t octet, int pos) {
			return scast<uint32_t>(octet) << (8 * pos);
		}
	} //namespace _ipv4_utils

	struct ipv4_t
	{
		uint32_t ipv4;

		constexpr ipv4_t(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
			using namespace _ipv4_utils;
			if constexpr (std::endian::native == std::endian::big) {
				this->ipv4 = left(a, 3) | left(b, 2) | left(c, 1) | left(d, 0);
			} else {
				this->ipv4 = left(a, 0) | left(b, 1) | left(c, 2) | left(d, 3);
			}
		}
		constexpr ipv4_t(in_addr addr) : ipv4{addr.s_addr} {}
		constexpr operator uint32_t() const { return ipv4; }
		constexpr operator in_addr() const { return {.s_addr = uint32_t(*this) }; };
	};


	struct port_t
	{
		uint16_t port;

		constexpr port_t(uint16_t port = 0) {
			if constexpr (std::endian::native == std::endian::big) {
				this->port = port;
			} else {
				this->port = std::byteswap(port);
			}
		}
		constexpr port_t(uint16_t port, int) : port{port} {}

		inline operator uint16_t() const { return port; }
	};

	struct ipv4 {
		static constexpr const ipv4_t any = ipv4_t{0, 0, 0, 0};
		static constexpr const ipv4_t localhost = ipv4_t{127, 0, 0, 1};
	};

	struct port {
		static constexpr const port_t any = port_t{0};
	};

	struct addr_t
	{
		ipv4_t ip;
		port_t port;

		constexpr explicit addr_t(ipv4_t ip = ipv4::any, port_t port = port::any)
			: ip{ip}, port{port}
		{}

		constexpr explicit addr_t(sockaddr_in addr)
			: ip{ipv4_t{addr.sin_addr}}
			, port{addr.sin_port, int()}
		{}

		inline operator sockaddr_in() const {
			return {
				.sin_family = AF_INET,
				.sin_port = port,
				.sin_addr = ip,
				.sin_zero = {0},
			};
		}
		addr_t& operator=(sockaddr_in addr) {
			ip = ipv4_t{addr.sin_addr};
			port = port_t{addr.sin_port, int()};
			return *this;
		}
	};

} //namespace pnd

template<>
struct std::formatter<pnd::ipv4_t> : std::formatter<std::string>
{
	auto format(pnd::ipv4_t ip, format_context& ctx) const {
		std::string fmt;
		if constexpr (std::endian::native == std::endian::big) {
			fmt =  std::format(
				"{}.{}.{}.{}",
				ip.ipv4 >> 24 & 0xFF,
				ip.ipv4 >> 16 & 0xFF,
				ip.ipv4 >> 8 & 0xFF,
				ip.ipv4 & 0xFF
			);
		} else {
			fmt = std::format(
				"{}.{}.{}.{}",
				ip.ipv4 & 0xFF,
				ip.ipv4 >> 8 & 0xFF,
				ip.ipv4 >> 16 & 0xFF,
				ip.ipv4 >> 24 & 0xFF
			);
		}
		return formatter<std::string>::format(fmt, ctx);
	}
};

template<>
struct std::formatter<pnd::port_t> : std::formatter<std::string>
{
	auto format(pnd::port_t port, format_context& ctx) const {
		if constexpr (std::endian::native == std::endian::little) {
			port.port = std::byteswap(port.port);
		}
		return formatter<std::string>::format(std::format("{}", port.port), ctx);
	}
};

template<>
struct std::formatter<pnd::addr_t> : std::formatter<std::string>
{
	auto format(pnd::addr_t addr, format_context& ctx) const {
		return formatter<std::string>::format(
			std::format("{}:{}", addr.ip, addr.port),
			ctx
		);
	}
};
