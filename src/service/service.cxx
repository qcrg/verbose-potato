#include "service.hxx"

#include "commands/commands_registry.hxx"
#include "epoll/epoll.hxx"
#include "sock.hxx"

#include "jlog.hxx"

#include <cassert>
#include <string_view>
#include <unordered_map>

namespace sv = std::ranges::views;

namespace pnd::service {

	struct ServiceImpl
	{
		std::unique_ptr<Epoll> epoll;
		std::unique_ptr<Sock> tcp_sock;
		std::unique_ptr<Sock> udp_sock;
		std::unordered_map<int, std::unique_ptr<Sock>> connections = {};
		size_t total_connections = 0;

		Exp<void> register_connection(Sock&& sock) {
			return register_connection(std::make_unique<Sock>(std::move(sock)));
		}

		Exp<void> register_connection(std::unique_ptr<Sock> sock) {
			auto [it, _] = connections.insert({
				sock->underlying_handle(),
				std::move(sock)
			});
			Sock* sock_ptr = it->second.get();
			total_connections++;
			auto peer = sock_ptr->get_peer();
			if (!peer)
				return Error(peer.error());
			jlog::info("Connected {}", addr_t{peer.value()});
			return epoll->insert(*sock_ptr, sock_ptr);
		}

		Exp<void> unregister_connection(Sock* sock) {
			auto peer = sock->get_peer();
			if (!peer)
				return Error(peer.error());
			jlog::info("Disconnected {}", addr_t{peer.value()});
			auto res = epoll->remove(*sock);
			size_t count = connections.erase(sock->underlying_handle());
			assert(count == 1);
			return res;
		}

		bool is_connection(const Sock* sock) {
			return connections.find(sock->underlying_handle()) != connections.end();
		}
	};

	const size_t MSG_MAX_LOG_LEN = 32;

	Exp<void> do_service(Sock* sock, const CommandsRegistry& reg) {
		std::byte buf[2048];
		std::byte out_buf[128];

		for (;;) {
			Exp<recv_from_t> resp = sock->recv_from(buf);
			if (!resp)
				return Error::join("receive", resp.error());
			if (resp->buf.size() == 0)
				return {};
			std::string_view sview = make_sview(resp->buf);
			jlog::info(
				"New message from {} '{}'{}",
				resp->addr,
				sview | sv::take(MSG_MAX_LOG_LEN),
				sview.size() > MSG_MAX_LOG_LEN ? "..." : ""
			);
			if (sview[0] == '/') {
				size_t sep_pos = sview.find_first_of(" \t\n\f\r\v", 1);
				std::string_view name = sview.substr(1, sep_pos - 1);
				Exp<const Cmd&> cmd = reg.get(std::string(name));
				if (cmd) {
					Exp<buf_t> res = cmd->get().call(out_buf, {});
					if (!res)
						return Error::join("cmd", res.error());
					res = sock->send_to(res.value(), resp->addr);
					if (!res)
						return Error::join("send", res.error());
					continue;
				} else {
					jlog::warn("{}", cmd.error());
				}
			}
			Exp<buf_t> res = sock->send_to(resp->buf, resp->addr);
			if (!res)
				return Error::join("send", res.error());
		}
	}

	Exp<void> do_connection_service(Sock* sock, ServiceImpl* srv) {
		Exp<Sock> client = sock->accept();
		if (!client)
			return Error(client.error());

		return srv->register_connection(std::move(client.value()));
	}

	Service::Service(ServiceImpl* impl)
		: impl{std::move(impl)}
	{}

	Service::Service(Service&& other)
		: impl{std::move(other.impl)}
	{
		other.impl = nullptr;
	}

	Service::~Service() {
		if (impl != nullptr)
			delete impl;
	}

	Service& Service::operator=(Service&& other) {
		std::swap(impl, other.impl);
		if (other.impl != nullptr)
			delete other.impl;
		other.impl = nullptr;
		return *this;
	}

	Exp<std::unique_ptr<Sock>> make_tcp(ipv4_t ip, port_t port) {
		Exp<Sock> sock = Sock::make(SockType::tcp, ip, port);
		if (!sock.has_value())
			return Error::join("tcp sock", sock.error());
		auto ok = sock->bind();
		if (!ok)
			return Error::join("Failed bind TCP Sock", ok.error());
		ok = sock->listen(5);
		if (!ok)
			return Error::join("Failed listen TCP Sock", ok.error());
		return std::make_unique<Sock>(std::move(sock.value()));
	}

	Exp<std::unique_ptr<Sock>> make_udp(ipv4_t ip, port_t port) {
		Exp<Sock> sock = Sock::make(SockType::udp, ip, port);
		if (!sock.has_value())
			return Error::join("udp sock", sock.error());
		auto ok = sock->bind();
		if (!ok)
			return Error::join("Failed bind UDP Sock", ok.error());
		return std::make_unique<Sock>(std::move(sock.value()));
	}

	Exp<std::unique_ptr<Epoll>> make_epoll() {
		Exp<Epoll> epoll = Epoll::make();
		if (!epoll)
			return Error::join("Epoll", epoll.error());
		return std::make_unique<Epoll>(std::move(epoll.value()));
	}

	Exp<Service> Service::make(ipv4_t ip, port_t port) {
		Exp<std::unique_ptr<Epoll>> epoll = make_epoll();
		if (!epoll)
			return Error(epoll.error());

		Exp<std::unique_ptr<Sock>> tcp = make_tcp(ip, port);
		if (!tcp)
			return Error(tcp.error());

		Exp<std::unique_ptr<Sock>> udp = make_udp(ip, port);
		if (!udp)
			return Error(udp.error());

		Exp<void> ok = (*epoll)->insert(*tcp->get(), tcp->get());
		if (!ok)
			return Error::join("insert tcp into Epoll", ok.error());
		ok = (*epoll)->insert(*udp->get(), udp->get());
		if (!ok)
			return Error::join("insert udp into Epoll", ok.error());
		auto srv = Service(new ServiceImpl(
			std::move(epoll.value()),
			std::move(tcp.value()),
			std::move(udp.value())
		));
		return srv;
	}

	Exp<void> Service::run(const CommandsRegistry& reg) {
		bool quit = false;
		while (!quit) {
			Exp<Sock*> sock = impl->epoll->wait<Sock*>();
			if (!sock)
				return Error::join("epoll.wait", sock.error());
			Exp<void> ok;
			if (
				sock.value()->underlying_handle() ==
				impl->tcp_sock->underlying_handle()
			) {
				ok = do_connection_service(sock.value(), impl);
			} else {
				ok = do_service(sock.value(), reg);
			}
			if (!ok)
				return Error::join("task.process", ok.error());
			if (impl->is_connection(sock.value())) {
				ok = impl->unregister_connection(sock.value());
				if (!ok)
					return ok;
			}
		}
		return {};
	}

	ConnectionStats Service::get_connections_stat() const {
		return {
			.current_connected = impl->connections.size(),
			.total_connections = impl->total_connections
		};
	}

} //namespace pnd::service
