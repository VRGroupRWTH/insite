cmake_minimum_required(VERSION 3.14)
project(insite-access-node)
include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DASIO_STANDALONE -pthread")

set(CMAKE_PREFIX_PATH CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/_deps/asio-src/asio/include")

FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio
    GIT_TAG asio-1-24-0
)

FetchContent_Declare(
    websocketpp
    GIT_REPOSITORY https://github.com/zaphoyd/websocketpp
    GIT_TAG master
)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG yaml-cpp-0.7.0
)

FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG master
)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.10.0
)

FetchContent_Declare(
    rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson
    GIT_TAG master
)

FetchContent_Declare(
  flatbuffersFC
    GIT_REPOSITORY https://github.com/google/flatbuffers
    GIT_TAG v2.0.6
)

FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow
    GIT_TAG master
)

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.1.0
)

FetchContent_MakeAvailable(
  asio
  websocketpp
  yaml-cpp
  cpr
  spdlog
  rapidjson
  Crow
  flatbuffersFC
  tomlplusplus
)


include_directories("extern/pdqsort")
set(ASIO_INCLUDE_DIR ${asio_SOURCE_DIR}/asio/include)

add_executable(
  insite-access-node
  main.cpp
  include/insite/better_params.h
  include/insite/query_params.h
  include/insite/spike_util.h
  include/insite/circular_timeseries.h
  include/insite/config.h config.cpp
  include/insite/jsonParameters.h jsonParameters.cpp
  include/insite/jsonStrings.h
  include/insite/nest/nestKernelStatus.h nest/kernelStatus.cpp
  include/insite/nest/nestMultimeter.h  nest/nestMultimeter.cpp
  include/insite/nest/nestMultimeterContainer.h nest/nestMultimeterContainer.cpp
  include/insite/nest/nestNode.h nest/nestNode.cpp
  include/insite/nest/nestSpikeDetectors.h
  include/insite/nest/nestSpikeRecorders.h
  include/insite/nest/nestSpikes.h nest/nestSpikes.cpp
  include/insite/nest/nestVersion.h nest/nestVersion.cpp
  include/insite/opcodes.h
  include/insite/params.h params.cpp
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
)

target_include_directories(insite-access-node INTERFACE ${asio_SOURCE_DIR}/asio/include)
target_include_directories(insite-access-node PRIVATE include/insite)
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
)

target_include_directories(
  insite-access-node
  PRIVATE
    ${CMAKE_BINARY_DIR}/_deps/rapidjson-src/include
    ${CMAKE_BINARY_DIR}/_deps/websocketpp-src
    ${CMAKE_BINARY_DIR}/_deps/crow-src/include
)
