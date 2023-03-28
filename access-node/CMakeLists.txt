cmake_minimum_required(VERSION 3.14)
project(insite-access-node)
include(FetchContent)
include(CPM.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DASIO_STANDALONE -pthread")
set(ASIO_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/asio/asio/include)

CPMAddPackage("gh:jbeder/yaml-cpp#yaml-cpp-0.7.0")
CPMAddPackage("gh:libcpr/cpr#master")
CPMAddPackage("gh:zaphoyd/websocketpp#master")
CPMAddPackage("gh:gabime/spdlog@1.10.0")
CPMAddPackage("gh:google/flatbuffers@2.0.6")
CPMAddPackage("gh:marzer/tomlplusplus@3.1.0")
CPMAddPackage("gh:OlivierLDff/asio.cmake@1.1.3")
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

option(INSITE_PRETTY_JSON OFF "Pretty JSON")

if (INSITE_PRETTY_JSON) 
message("Insite Pretty Json On")
add_compile_definitions(INSITE_PRETTY_JSON)
endif()

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/schema_generated.h
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/include
    COMMAND flatc --cpp  -o ${CMAKE_CURRENT_BINARY_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include/insite/flatbuffer-schema/schema.fbs
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/include/insite/flatbuffer-schema/schema.fbs
)

include_directories("extern/pdqsort")

add_executable(
  insite-access-node
  src/main.cpp

  ${CMAKE_CURRENT_BINARY_DIR}/include/schema_generated.h
  include/insite/circular_timeseries.h
  include/insite/config.h src/config.cpp
  include/insite/opcodes.h
  include/insite/query_params.h
  include/insite/query_string_builder.h src/query_string_builder.cpp
  include/insite/resource_flags.h
  include/insite/spike_util.h src/spike_util.cpp
  include/insite/utility_functions.h src/utility_functions.cpp
  include/insite/websocket_sender.h src/websocket_sender.cpp
  include/insite/websocket_server.h src/websocket_server.cpp

  include/insite/nest/endpoints.h src/nest/endpoints.cpp
  include/insite/nest/handler.h src/nest/handler.cpp
  include/insite/nest/json_strings.h
  include/insite/nest/kernel_status.h src/nest/kernel_status.cpp
  include/insite/nest/multimeter.h  src/nest/multimeter.cpp
  include/insite/nest/multimeter_container.h src/nest/multimeter_container.cpp
  include/insite/nest/nodecollections.h src/nest/nodecollections.cpp
  include/insite/nest/spike_container.h src/nest/spike_container.cpp
  include/insite/nest/spikerecorder_container.h  src/nest/spikerecorder_container.cpp
  include/insite/nest/spikerecorders.h  src/nest/spikerecorders.cpp
  include/insite/nest/spikes.h src/nest/spikes.cpp
  include/insite/nest/version.h src/nest/version.cpp

  include/insite/arbor/arbor_strings.h src/arbor/arbor_strings.cpp
  include/insite/arbor/cell_endpoints.h src/arbor/cell_endpoints.cpp
  include/insite/arbor/parameters.h src/arbor/parameters.cpp
  include/insite/arbor/probe_endpoints.h src/arbor/probe_endpoints.cpp
  include/insite/arbor/spike_endpoints.h src/arbor/spike_endpoints.cpp
  include/insite/arbor/arbor_endpoints.h src/arbor/arbor_endpoints.cpp

  include/insite/tvb/endpoints.h src/tvb/endpoints.cpp
  include/insite/tvb/handler.h src/tvb/handler.cpp
  include/insite/tvb/monitor.h src/tvb/monitor.cpp

)

set_property(
  TARGET insite-access-node 
  PROPERTY CXX_STANDARD 17
)

target_include_directories(
  insite-access-node
  PRIVATE
  include/insite
  ${CMAKE_BINARY_DIR}/include
  ${CMAKE_BINARY_DIR}/_deps/rapidjson-src/include
  ${CMAKE_BINARY_DIR}/_deps/websocketpp-src
)

target_link_libraries(
  insite-access-node
  PRIVATE
  cpr::cpr
  spdlog::spdlog
  Crow::Crow
  ssl
  crypto
  flatbuffers
  yaml-cpp::yaml-cpp
  tomlplusplus::tomlplusplus
  optional
  asio::asio
)
