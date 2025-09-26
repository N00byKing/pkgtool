cmake_minimum_required(VERSION 4.1.1)

project(pkgtool CXX C)

set(CMAKE_CXX_STANDARD 23)

add_executable(pkgtool
    pkg_type.cpp
    pkg.cpp
    common/io_file.cpp
    common/path_util.cpp
    crypto/crypto.cpp
)

# CryptoPP
set(CRYPTOPP_INSTALL OFF)
set(CRYPTOPP_BUILD_TESTING OFF)
set(CRYPTOPP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cryptopp)
# cryptopp instruction set checks do not account for added compile options,
# so disable extensions in the library config to match our chosen target CPU.
set(CRYPTOPP_DISABLE_AESNI ON)
set(CRYPTOPP_DISABLE_AVX2 ON)
add_subdirectory(ext-cryptopp-cmake)
file(COPY cryptopp DESTINATION cryptopp FILES_MATCHING PATTERN "*.h")
# remove externals/cryptopp from include directories because it contains a conflicting zlib.h file
set_target_properties(cryptopp PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/cryptopp")

# zlib-ng
find_package(zlib-ng REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(pkgtool cryptopp::cryptopp zlib-ng::zlib)

install(TARGETS pkgtool)
