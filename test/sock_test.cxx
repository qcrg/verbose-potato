#include "sock.hxx"

#include "doctest.h"

using namespace pnd;

TEST_CASE("TCP Sock") {
	SUBCASE("Creation") {
		CHECK(Sock::make(SockType::tcp).has_value());
		CHECK(Sock::make(SockType::tcp, ipv4_t{127, 0, 0, 1}).has_value());
		CHECK(Sock::make(SockType::tcp, ipv4::any, 25).has_value());
	}

	SUBCASE("Binding") {
		Exp<Sock> exp_sock = Sock::make(SockType::tcp);
		REQUIRE(exp_sock.has_value());
		Sock sock = std::move(exp_sock.value());
	}

	SUBCASE("Get host") {
	}
}

TEST_CASE("UDP Sock") {
	SUBCASE("get_host") {
		Exp<Sock> sock = Sock::make(SockType::udp, ipv4::localhost, 58439);
		REQUIRE(sock);
		auto ok = sock->bind();
		REQUIRE_MESSAGE(ok, ok.error());
		Exp<addr_t> addr = sock->get_host();
		REQUIRE(addr);
		CHECK(addr->ip.ipv4 == ipv4::localhost.ipv4);
		CHECK(addr->port.port == port_t{58439}.port);
	}

	SUBCASE("send") {
		Exp<Sock> server = Sock::make(SockType::udp);
		REQUIRE(server);
		REQUIRE(server->bind());
		Exp<addr_t> host = server->get_host();
		REQUIRE(host);
		Exp<Sock> client = Sock::make(SockType::udp, host.value());
		REQUIRE(client);

		std::byte buf[32] = {
			std::byte('h'),
			std::byte('e'),
			std::byte('l'),
			std::byte('l'),
			std::byte('o'),
			std::byte(','),
			std::byte(' '),
			std::byte('w'),
			std::byte('o'),
			std::byte('r'),
			std::byte('l'),
			std::byte('d'),
			std::byte('!'),
		};
		auto send_to = client->send_to(buf, host.value());
		REQUIRE_MESSAGE(send_to, send_to.error());
		CHECK(send_to->size() == 0);
	}
}
