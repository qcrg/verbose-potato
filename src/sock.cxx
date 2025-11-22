#include "sock.hxx"

#include "cast.hxx"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace pnd {

	Sock::Sock(Sock&& o)
		: sock{o.sock}
		, addr{o.addr}
	{
		o.sock = -1;
	}

	Sock& Sock::operator=(Sock&& o) {
		::close(sock);
		sock = o.sock;
		o.sock = -1;
		addr = o.addr;
		return *this;
	}

	Sock::~Sock() {
		::close(sock);
	}

	Exp<Sock> Sock::make(SockType type, ipv4_t ip, port_t port) {
		int sock = socket(AF_INET, scast<int>(type), 0);
		if (sock == -1)
			return Error::join("'socket'", errno);
		return Sock{sock, ip, port};
	}

	Exp<Sock> Sock::make(SockType type, addr_t addr) {
		return Sock::make(type, addr.ip, addr.port);
	}

	Exp<void> Sock::bind() {
		sockaddr_in addr = this->addr;
		if (::bind(sock, rcast<sockaddr*>(&addr), sizeof(addr)) < 0)
			return Error::join("'bind'", errno);
		return {};
	}

	Exp<void> Sock::listen(int queue_size) {
		if (::listen(sock, queue_size) < 0)
			return Error::join("'listen'", errno);
		return {};
	}

	Exp<void> Sock::connect() {
		sockaddr_in addr = this->addr;
		if (::connect(sock, rcast<sockaddr*>(&addr), sizeof(addr)) < 0)
			return Error::join("'connect'", errno);
		return {};
	}

	Exp<Sock> Sock::accept() {
		sockaddr_in addr;
		socklen_t len = sizeof(addr);
		int client_fd = ::accept(sock, rcast<sockaddr*>(&addr), &len);
		if (client_fd < 0)
			return Error::join("'accept'", errno);
		return Sock{client_fd, ipv4_t{addr.sin_addr}, port_t{addr.sin_port, int()}};
	}

	Exp<buf_t> Sock::recv(buf_t buf) {
		ssize_t len = ::recv(sock, buf.data(), buf.size(), 0);
		if (len == -1)
			return Error::join("'recv'", errno);
		return buf.subspan(0, len);
	}

	Exp<buf_t> Sock::send(buf_t buf) {
		ssize_t len = ::send(sock, buf.data(), buf.size(), 0);
		if (len == -1)
			return Error::join("'send'", errno);
		return buf.subspan(len, buf.size() - len);
	}

	Exp<recv_from_t> Sock::recv_from(buf_t buf) {
		sockaddr_in addr;
		socklen_t addr_len = sizeof(addr);
		ssize_t len = ::recvfrom(
			sock,
			buf.data(),
			buf.size(),
			0,
			rcast<sockaddr*>(&addr),
			&addr_len
		);
		if (len == -1)
			return Error::join("'recvfrom'", errno);
		return recv_from_t{buf.subspan(0, len), addr_t{addr}};
	}

	Exp<buf_t> Sock::send_to(buf_t buf, const addr_t& addr) {
		sockaddr_in sock_addr = addr;
		ssize_t len = ::sendto(
			sock,
			buf.data(),
			buf.size(),
			0,
			rcast<sockaddr*>(&sock_addr),
			sizeof(sock_addr)
		);
		if (len == -1)
			return Error::join("'sendto'", errno);
		return buf.subspan(len, buf.size() - len);
	}

	Exp<addr_t> Sock::get_host() const {
		sockaddr_in addr;
		socklen_t len = sizeof(addr);
		if (::getsockname(sock, rcast<sockaddr*>(&addr), &len) < 0)
			return Error::join("getsockname", errno);
		return addr_t{ipv4_t{addr.sin_addr}, port_t{addr.sin_port, int()}};
	}

	Exp<addr_t> Sock::get_peer() const {
		sockaddr_in addr;
		socklen_t len = sizeof(addr);
		if (::getpeername(sock, rcast<sockaddr*>(&addr), &len) < 0)
			return Error::join("getpeername", errno);
		return addr_t{ipv4_t{addr.sin_addr}, port_t{addr.sin_port, int()}};
	}

} //namespace pnd
