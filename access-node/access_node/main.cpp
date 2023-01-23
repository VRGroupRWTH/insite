#define CROW_MAIN
#include <config.h>
#include <crow/app.h>
#include <crow/routing.h>
#include <nest/nestKernelStatus.h>
#include <nest/nestMultimeter.h>
#include <nest/nestNode.h>
#include <nest/nestSpikeRecorders.h>
#include <nest/nestSpikes.h>
#include <nest/nestVersion.h>
#include <tvb_handler.h>
#include "nest_handler.h"
#include <tvb_http_endpoints.h>
#include <websocket_server.h>
#include <exception>
#include <limits>
#include <websocketpp/config/asio_no_tls.hpp>
#include "spdlog/spdlog.h"
#include "nest/nest_endpoints.h"

using Server = websocketpp::server<websocketpp::config::asio>;

using MessagePtr = Server::message_ptr;

int main() {
  using namespace insite;
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%s:%!:%#] %v");
  ServerConfig::GetInstance().ParseConfigIfExists();
  ServerConfig::GetInstance().GenerateRequestUrls();

  crow::SimpleApp app;
  WebsocketServer srv(ServerConfig::GetInstance().port_number_access_ws + 1);
  TvbHandler tvb_handler;
  NestHandler nest_handler;
  // TVBHttpEndpoint::tvb_handler = &tvb_handler;
  // TVBHttpEndpoint tvb_http_endpoint;
  srv.SetTvbHandler(&tvb_handler);
  srv.SetNestHandler(&nest_handler);
  srv.StartServer();
  tvb_handler.StartThread();
  nest_handler.StartThread();
  nest_handler.ws_server = &srv;

  // TVBHttpEndpoint::RegisterRoutes(app);
  NestHttpEndpoint::RegisterRoutes(app);
  // clang-format off
  CROW_ROUTE(app, "/version")(Version);
  // clang-format on

  app.stream_threshold(std::numeric_limits<unsigned int>::max());
  app.port(ServerConfig::GetInstance().port_number_access);
  app.run();
  srv.StopServer();
  spdlog::debug("Stopped WebsocketServer");
  tvb_handler.StopThread();
  nest_handler.StopThread();
}
