#include "addr.hxx"

#include "doctest.h"

#include <cstring>
#include <arpa/inet.h>
#include <format>

using namespace pnd;

TEST_CASE("ipv4_t") {
	const char* ip_str = "192.168.0.1";
	const pnd::ipv4_t ip(192, 168, 0, 1);

	sockaddr_in addr;
	::memset(&addr, 0, sizeof(addr));
	REQUIRE(inet_pton(AF_INET, ip_str, &addr.sin_addr) > 0);

	CHECK(addr.sin_addr.s_addr == ip.ipv4);

	SUBCASE("from in_addr") {
		in_addr idr{12345678};
		ipv4_t ip{idr};
		CHECK(idr.s_addr == ip.ipv4);
	}

	SUBCASE("format print") {
		CHECK(std::format("{}", ip) == std::string(ip_str));
	}

}

TEST_CASE("port_t") {
	const int raw_port = 6789;
	const pnd::port_t port{raw_port};

	CHECK(port.port == htons(raw_port));

	SUBCASE("format print") {
		CHECK(std::format("{}", port) == std::format("{}", raw_port));
	}
}

TEST_CASE("addr_t") {
	const char* ip_str = "192.168.0.1";
	const int raw_port = 6789;

	sockaddr_in sock_addr;
	::memset(&sock_addr, 0, sizeof(sock_addr));
	REQUIRE(inet_pton(AF_INET, ip_str, &sock_addr.sin_addr) > 0);
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(raw_port);

	pnd::addr_t addr{ipv4_t{192, 168, 0, 1}, port_t{raw_port}};
	sockaddr_in other_sock_addr = addr;

	CHECK(::memcmp(&sock_addr, &other_sock_addr, sizeof(sockaddr_in)) == 0);

	SUBCASE("from sockaddr_in") {
		sockaddr_in sadr = addr;
		addr_t odr{sadr};
		CHECK(odr.ip == addr.ip);
		CHECK(odr.port == addr.port);
	}
}
