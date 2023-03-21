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
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include
    COMMAND flatc --cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/insite/flatbuffer-schema/schema.fbs
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/include/insite/flatbuffer-schema/schema.fbs
)

include_directories("extern/pdqsort")

add_executable(
  insite-access-node
  main.cpp
  include/insite/query_params.h
  include/insite/spike_util.h
  include/insite/circular_timeseries.h
  include/insite/config.h config.cpp
  # include/insite/jsonParameters.h jsonParameters.cpp
  include/insite/jsonStrings.h
  include/insite/nest/nestKernelStatus.h nest/kernelStatus.cpp
  include/insite/nest/nestMultimeter.h  nest/nestMultimeter.cpp
  include/insite/nest/nestMultimeterContainer.h nest/nestMultimeterContainer.cpp
  include/insite/nest/nestNode.h nest/nestNode.cpp
  include/insite/nest/nestSpikeDetectors.h
  include/insite/nest/nestSpikeRecorders.h
  include/insite/nest/nestSpikes.h nest/nestSpikes.cpp
  include/insite/nest/nestVersion.h nest/nestVersion.cpp
  include/insite/arbor/cell_endpoints.h arbor/cell_endpoints.cpp
  include/insite/arbor/probe_endpoints.h arbor/probe_endpoints.cpp
  include/insite/arbor/spike_endpoints.h arbor/spike_endpoints.cpp
  include/insite/arbor/parameters.h arbor/parameters.cpp
  include/insite/arbor/arbor_strings.h arbor/arbor_strings.cpp
  include/insite/opcodes.h
  # include/insite/params.h params.cpp
  include/insite/queryStringBuilder.h
  include/insite/resource_flags.h
  include/insite/spike.h nest/spike.cpp
  include/insite/spikedetector.h nest/spikedetector.cpp
  include/insite/tvb_http_endpoints.h
  include/insite/tvbMonitor.h tvbMonitor.cpp
  include/insite/tvb_handler.h tvb_handler.cpp
  include/insite/nest_handler.h nest_handler.cpp
  include/insite/utilityFunctions.h utilityFunctions.cpp
  include/insite/websocket_sender.h websocket_sender.cpp
  include/insite/websocket_server.h websocket_server.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/include/schema_generated.h
)

target_include_directories(insite-access-node PRIVATE include/insite)
target_include_directories(insite-access-node PRIVATE ${CMAKE_BINARY_DIR}/include)
set_property(
  TARGET insite-access-node 
  PROPERTY CXX_STANDARD 17
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

target_include_directories(
  insite-access-node
  PRIVATE
    ${CMAKE_BINARY_DIR}/_deps/rapidjson-src/include
    ${CMAKE_BINARY_DIR}/_deps/websocketpp-src
    # ${asio_INCLUDE_DIR}/include
)

message("${ASIO_INCLUDE_DIR}")
