cmake_minimum_required(VERSION 3.15)
project(insite-arbor)

find_package(arbor REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(asio_wrapper INTERFACE)
target_include_directories(asio_wrapper INTERFACE ${CMAKE_BINARY_DIR}/_deps/asio-src/asio/include)
add_library(asio::asio ALIAS asio_wrapper)

include(CPM.cmake)
CPMAddPackage("gh:gabime/spdlog@1.10.0")
CPMAddPackage("gh:chriskohlhoff/asio#asio-1-24-0")
CPMAddPackage("gh:TartanLlama/optional@1.0.0")

CPMAddPackage(
    NAME rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson
    GIT_TAG master
    DOWNLOAD_ONLY TRUE
    EXCLUDE_FROM_ALL TRUE
)

CPMAddPackage(
    NAME Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow
    GIT_TAG master
    EXCLUDE_FROM_ALL TRUE
    OPTIONS "CROW_INSTALL OFF"
)

set(CMAKE_CXX_STANDARD 17)

add_library(insite-arbor 
    include/insite/arbor/pipeline_backend.h src/pipeline_backend.cpp 
    include/insite/arbor/serialize.h src/serialize.cpp
    include/insite/arbor/arbor_spike.h src/arbor_spike.cpp
    include/insite/arbor/arbor_probe.h src/arbor_probe.cpp
    include/insite/arbor/optionals_util.h src/optionals_util.cpp
    include/insite/arbor/parameters.h src/parameters.cpp
)
target_include_directories(insite-arbor 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
        ${CMAKE_BINARY_DIR}/include
)
target_link_libraries(insite-arbor
    PUBLIC 
    arbor::arbor-public-deps
    spdlog::spdlog_header_only
    arbor::arbor
    Crow::Crow
    ssl
    crypto
    optional
)

target_include_directories(
  insite-arbor
  PUBLIC
    ${CMAKE_BINARY_DIR}/_deps/rapidjson-src/include
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/insite-arbor-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/insite-arbor-config.cmake)

install(TARGETS insite-arbor)
install(DIRECTORY include/insite TYPE INCLUDE)
install(
    DIRECTORY 
        ${CMAKE_BINARY_DIR}/_deps/rapidjson-src/include/ 
        ${CMAKE_BINARY_DIR}/_deps/asio-src/asio/include/
        ${CMAKE_BINARY_DIR}/_deps/spdlog-src/include/
        ${CMAKE_BINARY_DIR}/_deps/optional-src/include/
        ${CMAKE_BINARY_DIR}/_deps/crow-src/include/
    DESTINATION include/insite/extern
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/insite-arbor-config.cmake DESTINATION .)
