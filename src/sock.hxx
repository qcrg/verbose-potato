#pragma once

#include "error.hxx"
#include "buf.hxx"
#include "addr.hxx"

#include <netinet/in.h>

namespace pnd {

	enum class SockType
	{
		tcp = SOCK_STREAM,
		udp = SOCK_DGRAM,
	};

	struct recv_from_t
	{
		buf_t buf;
		addr_t addr;
	};

	class Sock
	{
		int sock;
		addr_t addr;

		Sock(int sock, ipv4_t ip, port_t port)
			: sock{sock}
			, addr{ip, port}
		{}

	public:
		Sock(const Sock&) = delete;
		Sock(Sock&& sock);

		~Sock();
		Sock& operator=(const Sock& other) = delete;
		Sock& operator=(Sock&& other);

		static Exp<Sock> make(
			SockType type,
			ipv4_t ip = ipv4::localhost,
			port_t port = port::any
		);
		static Exp<Sock> make(SockType type, addr_t addr);

		inline int underlying_handle() { return sock; }
		inline int underlying_handle() const { return sock; }

		Exp<void> bind();
		Exp<void> listen(int queue_size);
		Exp<void> connect();
		Exp<Sock> accept();

		Exp<buf_t> recv(buf_t buf);
		Exp<buf_t> send(buf_t buf);
		Exp<recv_from_t> recv_from(buf_t buf);
		Exp<buf_t> send_to(buf_t buf, const addr_t& addr);

		Exp<addr_t> get_host() const;
		Exp<addr_t> get_peer() const;
	};

} //namespace pnd
