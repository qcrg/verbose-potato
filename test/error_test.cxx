#include "error.hxx"

#include "doctest.h"

using namespace pnd;

TEST_CASE("Error creation") {
	SUBCASE("Error::join(...)") {
		CHECK(Error::join().error() == "");
		CHECK(Error::join("").error() == "");
		CHECK(Error::join("foo").error() == "foo");
		CHECK(Error::join(EAGAIN).error() == "Resource temporarily unavailable");
		CHECK(
			Error::join("some", EAGAIN).error() ==
			"some: Resource temporarily unavailable"
		);
	}

	SUBCASE("Error::joind(...)") {
		CHECK(Error::joind("").error() == "");
		CHECK(Error::joind("", "").error() == "");
		CHECK(Error::joind("", "", "").error() == "");
		CHECK(Error::joind(",", "", "").error() == ",");
		CHECK(
			Error::joind("; ", "some", EAGAIN).error() ==
			"some; Resource temporarily unavailable"
		);
	}
}
