###------<MAIN>
PROG_NAME ?= verbose-potato
BUILD_DIR ?= build
PROG := ${BUILD_DIR}/${PROG_NAME}

MAIN_SRC := src/main.cxx
SRCS := \
	src/sock.cxx \
	src/epoll/epoll.cxx \
	src/commands/commands_registry.cxx \
	src/commands/time/time.cxx \
	src/commands/shutdown/shutdown.cxx \
	src/commands/stats/stats.cxx \
	src/service/service.cxx
HEADERS := \
	src/jlog.hxx \
	src/addr.hxx \
	src/sock.hxx \
	src/cast.hxx \
	src/error.hxx \
	src/epoll/epoll.hxx \
	src/commands/commands_registry.hxx \
	src/commands/time/time.hxx \
	src/commands/stats/stats.hxx \
	src/commands/shutdown/shutdown.hxx
OBJS := $(addprefix ${BUILD_DIR}/,${SRCS:.cxx=.o} ${MAIN_SRC:.cxx=.o})
DIRS := $(dir ${OBJS})

FLAGS := -Wall -Wextra -Wpedantic -g -ggdb -Isrc -Ithird_party
CFLAGS += ${FLAGS}
CXXFLAGS += ${FLAGS} -std=c++23
#------</MAIN>

###------<COMPILER>
ifneq (, $(shell command -v ccache 2>/dev/null))
CCACHE ?= ccache
endif

ifneq (, $(shell command -v bear 2>/dev/null))
BEAR_TARGET := ${BUILD_DIR}/compile_commands.json
BEAR ?= bear --append --output ${BEAR_TARGET} --
endif

CC := ${BEAR} ${CCACHE} ${CC}
CXX := ${BEAR} ${CCACHE} ${CXX}
#------</COMPILER>

default: build

run: build
	./${BUILD_DIR}/${PROG_NAME}

build: create_dirs ${BUILD_DIR}/${PROG_NAME}

create_dirs:
	@mkdir -p ${DIRS}

${PROG}: ${OBJS} ${HEADERS}
	${CXX} ${CXXFLAGS} -o $@ ${OBJS}

${BUILD_DIR}/%.o: %.cxx ${HEADERS}
	${CXX} ${CXXFLAGS} -c -o $@ $<

###------<TEST>
TEST_PROG_NAME ?= test
TEST_BUILD_DIR ?= ${BUILD_DIR}/test
TEST_PROG := ${TEST_BUILD_DIR}/${TEST_PROG_NAME}

TEST_MAIN_SRC := test/main_test.cxx
TEST_SRCS := \
	${SRCS} \
	test/error_test.cxx \
	test/addr_test.cxx \
	test/sock_test.cxx \
	test/commands_test.cxx \
	test/time_test.cxx
TEST_HEADERS := \
	${HEADERS}
TEST_OBJS := $(addprefix ${BUILD_DIR}/,${TEST_SRCS:.cxx=.o} ${TEST_MAIN_SRC:.cxx=.o})
TEST_DIRS := $(dir ${TEST_OBJS})

TEST_FLAGS := ${FLAGS} -Ithird_party/doctest
TEST_CFLAGS += ${CFLAGS} ${TEST_FLAGS}
TEST_CXXFLAGS += ${CXXFLAGS} ${TEST_FLAGS}

test: build_test
	./${TEST_PROG}

build_test: create_test_dirs ${TEST_PROG}

create_test_dirs:
	@mkdir -p ${TEST_DIRS}

${TEST_PROG}: ${TEST_OBJS} ${TEST_HEADERS}
	${CXX} ${TEST_CXXFLAGS} -o $@ ${TEST_OBJS}

${TEST_BUILD_DIR}/%.o: test/%.cxx ${TEST_HEADERS}
	${CXX} ${TEST_CXXFLAGS} -c -o $@ $<
#------</TEST>

clean:
	${RM} -r ${BUILD_DIR} ${TEST_BUILD_DIR}

full_clean: clean
	${RM} -r .cache

.PHONY: build build_test clean full_clean run create_dirs create_test_dirs
