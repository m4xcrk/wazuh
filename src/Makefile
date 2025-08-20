# Copyright (C) 2015, Wazuh Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
uname_P := $(shell sh -c 'uname -p 2>/dev/null || echo not')
uname_R := $(shell sh -c 'uname -r 2>/dev/null || echo not')
uname_V := $(shell sh -c 'uname -v 2>/dev/null || echo not')
uname_M := $(shell sh -c 'uname -m 2>/dev/null || echo not')

# Platform detection and toolchain setup
ifeq (${TARGET}, winagent)
WAZUH_LIB_OUTPUT_PATH := win32/
STRIP_TOOL := i686-w64-mingw32-strip
DLLTOOL := i686-w64-mingw32-dlltool
libstdc++_path := $(shell sh -c 'i686-w64-mingw32-g++-posix --print-file-name=libstdc++-6.dll 2>/dev/null || echo not')
LIBSTDCPP_NAME := libstdc++-6.dll
libgcc_s_path := $(shell sh -c 'i686-w64-mingw32-g++-posix --print-file-name=libgcc_s_dw2-1.dll 2>/dev/null || echo not')
LIBGCC_S_NAME := libgcc_s_dw2-1.dll
else
ifeq (${uname_S},AIX)
libstdc++_path_temp := $(shell sh -c 'g++ --print-file-name=libstdc++.a 2>/dev/null || echo not')
libgcc_s_path_temp := $(shell sh -c 'g++ --print-file-name=libgcc_s.a 2>/dev/null || echo not')
libstdc++_path=$(subst libstdc++.a,pthread/libstdc++.a,$(libstdc++_path_temp))
libgcc_s_path=$(subst libgcc_s.a,pthread/libgcc_s.a,$(libgcc_s_path_temp))
LIBSTDCPP_NAME := libstdc++.a
LIBGCC_S_NAME := libgcc_s.a
else
libstdc++_path := $(shell sh -c 'g++ --print-file-name=libstdc++.so.6 2>/dev/null || echo not')
libgcc_s_path := $(shell sh -c 'g++ --print-file-name=libgcc_s.so.1 2>/dev/null || echo not')
LIBSTDCPP_NAME := libstdc++.so.6
LIBGCC_S_NAME := libgcc_s.so.1
endif
STRIP_TOOL := strip
endif

ifeq (, $(filter ${libstdc++_path}, not ${LIBSTDCPP_NAME}))
ifeq (, $(filter ${libgcc_s_path}, not ${LIBGCC_S_NAME}))
CPPLIBDEPS := ${LIBSTDCPP_NAME} ${LIBGCC_S_NAME}
endif
endif

HAS_CHECKMODULE = $(shell command -v checkmodule > /dev/null && echo YES)
HAS_SEMODULE_PACKAGE = $(shell command -v semodule_package > /dev/null && echo YES)
CHECK_ARCHLINUX := $(shell sh -c 'grep "Arch Linux" /etc/os-release > /dev/null && echo YES || echo not')
CHECK_CENTOS5 := $(shell sh -c 'grep "CentOS release 5." /etc/redhat-release 2>&1 > /dev/null && echo YES || echo not')
CHECK_ALPINE := $(shell sh -c 'grep "Alpine Linux" /etc/os-release 2>&1 > /dev/null && echo YES || echo not')

ARCH_FLAGS =

ROUTE_PATH := $(shell pwd)
EXTERNAL_JSON=external/cJSON/
EXTERNAL_ZLIB=external/zlib/
EXTERNAL_SQLITE=external/sqlite/
EXTERNAL_OPENSSL=external/openssl/
EXTERNAL_LIBYAML=external/libyaml/
EXTERNAL_CURL=external/curl/
EXTERNAL_AUDIT=external/audit-userspace/
EXTERNAL_LIBFFI=external/libffi/
EXTERNAL_LIBPLIST=external/libplist/
EXTERNAL_CPYTHON=external/cpython/
EXTERNAL_MSGPACK=external/msgpack/
EXTERNAL_BZIP2=external/bzip2/
EXTERNAL_GOOGLE_TEST=external/googletest/
EXTERNAL_GOOGLE_BENCHMARK=external/benchmark/
EXTERNAL_ROCKSDB=external/rocksdb/
EXTERNAL_RE2=external/re2/
EXTERNAL_ABSEILCPP=external/abseil-cpp/
EXTERNAL_FLATBUFFERS=external/flatbuffers/
EXTERNAL_FMT=external/fmt/
EXTERNAL_SPDLOG=external/spdlog/
EXTERNAL_RAPIDJSON=external/rapidjson/
EXTERNAL_RXCPP=external/RxCpp/
EXTERNAL_YAMLCPP=external/yaml-cpp/
EXTERNAL_PUGIXML=external/pugixml/
EXTERNAL_MAXMINDDB=external/libmaxminddb/
EXTERNAL_PROTOBUF=external/protobuf/
EXTERNAL_DATE=external/date/
EXTERNAL_FAST_FLOAT=external/fast_float/
EXTERNAL_TASKFLOW=external/taskflow/
EXTERNAL_CONCURRENTQUEUE=external/concurrentqueue/
EXTERNAL_LZMA=external/lzma/
EXTERNAL_LUA=external/lua/
EXTERNAL_LIBPCRE2=external/libpcre2/
EXTERNAL_SIMDJSON=external/simdjson/
ifneq (${TARGET},winagent)
EXTERNAL_PROCPS=external/procps/
EXTERNAL_LIBDB=external/libdb/build_unix/
EXTERNAL_PACMAN=external/pacman/
EXTERNAL_LIBARCHIVE=external/libarchive/
endif
EXTERNAL_JEMALLOC=external/jemalloc/
ifeq (${uname_S},Linux)
EXTERNAL_RPM=external/rpm/
EXTERNAL_POPT=external/popt/
EXTERNAL_LIBBPF=external/libbpf-bootstrap/
endif

# User and group defaults
WAZUH_GROUP?=wazuh
WAZUH_USER?=wazuh

# More variable declarations and options... (unchanged from your code)

# ... [Omitted for brevity; copy all unchanged variable assignments from your original]

####################
#### Target ########
####################

ifndef TARGET
	TARGET=failtarget
endif # TARGET

ifeq (${TARGET},agent)
	DEFINES+=-DCLIENT
endif

ifeq (${TARGET},local)
	DEFINES+=-DLOCAL
endif

# Build targets and rules, unchanged from your code...

.PHONY: build
build: ${TARGET}
ifneq (${TARGET},failtarget)
	${MAKE} settings
	@echo
	${QUIET_NOTICE}
	@echo "Done building ${TARGET}"
	${QUIET_ENDCOLOR}
endif
	@echo

.PHONY: failtarget
failtarget:
	@echo "TARGET is required: "
	@echo "   make TARGET=server   to build the server"
	@echo "   make TARGET=local      - local version of server"
	@echo "   make TARGET=hybrid     - hybrid version of server"
	@echo "   make TARGET=agent    to build the unix agent"
	@echo "   make TARGET=winagent to build the windows agent"

# ... [Omitted for brevity, keep all remaining targets and recipes as in your code]

# External build stubs (for missing/cut-off sections)
${RE2_LIB}: $(ABSEILCPP_LIB)
	mkdir -p $(EXTERNAL_RE2)build && \
	cd $(EXTERNAL_RE2)build && \
	cmake .. -DCMAKE_BUILD_TYPE=Release && \
	make

# Add any other missing or incomplete recipes here as stubs:
# Example:
build_gtest:
	@echo "Stub: build_gtest not implemented"

build_benchmark:
	@echo "Stub: build_benchmark not implemented"

build_python:
	@echo "Stub: build_python not implemented"

build_sysinfo:
	@echo "Stub: build_sysinfo not implemented"

build_shared_modules:
	@echo "Stub: build_shared_modules not implemented"

build_wazuh_cmake:
	@echo "Stub: build_wazuh_cmake not implemented"

build_syscollector:
	@echo "Stub: build_syscollector not implemented"

# End of file
