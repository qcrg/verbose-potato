#include "epoll.hxx"

#include <unistd.h>
#include <sys/epoll.h>

namespace pnd {

	using ptr_t = Epoll::ptr_t;

	Epoll::Epoll(Epoll&& o)
		: Epoll{o.fd}
	{
		o.fd = -1;
	}

	Epoll& Epoll::operator=(Epoll&& o) {
		::close(fd);
		fd = o.fd;
		o.fd = -1;
		return *this;
	}

	Epoll::~Epoll() {
		::close(fd);
	}

	Exp<Epoll> Epoll::make() {
		int fd = epoll_create1(0);
		if (fd == -1)
			return Error::join("'epoll_create1'", errno);
		return Epoll{fd};
	}

	Exp<void> Epoll::_insert_impl(int fd, ptr_t ptr) {
		epoll_event event = {
			.events = EPOLLIN,
			.data = {.ptr = ptr}
		};
		int res = epoll_ctl(
			this->fd,
			EPOLL_CTL_ADD,
			fd,
			&event
		);
		if (res == -1)
			return Error::join("'epoll_ctl'", errno);
		return {};
	}

	Exp<void> Epoll::_remove_impl(int fd) {
		int res = epoll_ctl(
			this->fd,
			EPOLL_CTL_DEL,
			fd,
			nullptr
		);
		if (res == -1)
			return Error::join("'epoll_ctl'", errno);
		return {};
	}

	Exp<ptr_t> Epoll::_wait_impl(int timeout) {
		epoll_event event;
		int len;
		do {
			len = epoll_wait(fd, &event, 1, timeout);
		} while (len == -1 && errno == EAGAIN);
		if (len == -1)
			return Error::join("'epoll_wait'", errno);
		return rcast<ptr_t>(event.data.ptr);
	}

} //namespace pnd
