#pragma once

#include "error.hxx"
#include "sock.hxx"
#include "cast.hxx"

#include <type_traits>

namespace pnd {

	class Epoll
	{
		int fd;

		constexpr Epoll(int fd) : fd{fd} {}
	public:
		using ptr_t = void*;

		Epoll(const Epoll&) = delete;
		Epoll(Epoll&& sock);

		~Epoll();
		Epoll& operator=(const Epoll& other) = delete;
		Epoll& operator=(Epoll&& other);

		static Exp<Epoll> make();

		inline int underlying_handle() { return fd; }

		template<typename T = void*>
		requires std::is_pointer_v<T>
		Exp<void> insert(Sock& sock, T ptr = nullptr) {
			return _insert_impl(sock.underlying_handle(), ptr);
		}

		Exp<void> remove(Sock& sock) {
			return _remove_impl(sock.underlying_handle());
		}

		template<typename T = void*>
		requires std::is_pointer_v<T>
		Exp<T> wait(int timeout = -1) {
			Exp<ptr_t> res = _wait_impl(timeout);
			if (!res)
				return Error(res.error());
			return rcast<T>(res.value());
		}

		Exp<void> _insert_impl(int fd, ptr_t ptr);
		Exp<void> _remove_impl(int fd);
		Exp<ptr_t> _wait_impl(int timeout);
	};

} //namespace pnd
