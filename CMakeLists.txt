CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(connect-udp)

IF(CMAKE_BUILD_TYPE STREQUAL "")
    SET(CMAKE_BUILD_TYPE Debug)
ENDIF()
MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
IF (CMAKE_BUILD_TYPE STREQUAL Debug)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
ELSEIF (CMAKE_BUILD_TYPE STREQUAL Debug)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
ENDIF()
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} $ENV{EXTRA_CFLAGS}")

MESSAGE(STATUS "Compiler flags: ${CMAKE_C_FLAGS}")

ADD_SUBDIRECTORY(boringssl)

INCLUDE_DIRECTORIES(boringssl/include)
INCLUDE_DIRECTORIES(lsquic/include)
INCLUDE_DIRECTORIES(lsquic/bin)
INCLUDE_DIRECTORIES(lsquic/src/liblsquic)       # For lsquic_xxhash.h
ADD_SUBDIRECTORY(lsquic/src/liblsquic)

# ADD_EXECUTABLE(udp_server udp_server.c)
add_executable(http_proxy_server http_proxy_server.c prog.c test_common.c test_cert.c)
add_executable(http_proxy_client http_proxy_client.c prog.c test_common.c test_cert.c)
SET(LIBS lsquic crypto ssl z event)
IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # If using older glibc, need to link with -lrt.  See clock_getres(2).
    EXECUTE_PROCESS(
        COMMAND ${PROJECT_SOURCE_DIR}/print-glibc-version.sh ${CMAKE_C_COMPILER}
        OUTPUT_VARIABLE GLIBC_VERSION)
    IF(NOT GLIBC_VERSION EQUAL "" AND GLIBC_VERSION VERSION_LESS 2.17)
        SET(LIBS ${LIBS} rt)
    ENDIF()
ENDIF()

TARGET_LINK_LIBRARIES(http_proxy_client ${LIBS})
TARGET_LINK_LIBRARIES(http_proxy_server ${LIBS})



INCLUDE(CheckFunctionExists)
CHECK_FUNCTION_EXISTS(sendmmsg HAVE_SENDMMSG)
CHECK_FUNCTION_EXISTS(recvmmsg HAVE_RECVMMSG)
CHECK_FUNCTION_EXISTS(open_memstream HAVE_OPEN_MEMSTREAM)


INCLUDE(CheckSymbolExists)

CHECK_SYMBOL_EXISTS(
    IP_MTU_DISCOVER
    "netinet/in.h"
    HAVE_IP_MTU_DISCOVER
)

CHECK_SYMBOL_EXISTS(
    IPV6_MTU_DISCOVER
    "netinet/in.h"
    HAVE_IPV6_MTU_DISCOVER
)

CHECK_SYMBOL_EXISTS(
    IP_DONTFRAG
    "netinet/in.h"
    HAVE_IP_DONTFRAG
)

CHECK_SYMBOL_EXISTS(
    preadv
    "sys/uio.h"
    HAVE_PREADV
)

INCLUDE(CheckIncludeFiles)

IF (MSVC AND PCRE_LIB)
FIND_PATH(EVENT_INCLUDE_DIR NAMES pcreposix.h)
IF (EVENT_INCLUDE_DIR)
    MESSAGE(STATUS "found pcreposix.h")
    SET(HAVE_REGEX 1)
ELSE()
    MESSAGE(FATAL_ERROR "event2/event.h was not found")
ENDIF()
ELSE()
CHECK_INCLUDE_FILES(regex.h HAVE_REGEX)
ENDIF()

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/test_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/test_config.h)


