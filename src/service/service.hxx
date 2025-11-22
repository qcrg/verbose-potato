#pragma once

#include "addr.hxx"
#include "commands/commands_registry.hxx"
#include "connection_stats.hxx"

namespace pnd::service {

	struct ServiceImpl;

	class Service
	{
		ServiceImpl* impl;

		Service(ServiceImpl* impl);
	public:
		Service(const Service& other) = delete;
		Service& operator=(const Service& other) = delete;
		Service(Service&& other);
		Service& operator=(Service&& other);
		~Service();

		static Exp<Service> make(ipv4_t ip, port_t port);

		Exp<void> run(const CommandsRegistry& reg);
		ConnectionStats get_connections_stat() const;
	};

} //namespace pnd::service
