#define CROW_MAIN
#include "arbor/arbor_endpoints.h"
#include "crow/middlewares/cors.h"
#include "spdlog/cfg/env.h"
#include "spdlog/spdlog.h"
#include <config.h>
#include <crow/app.h>
#include <crow/routing.h>
#include <exception>
#include <limits>
#include <nest/endpoints.h>
#include <nest/handler.h>
#include <nest/spikerecorders.h>
#include <nest/spikes.h>
#include <tvb/endpoints.h>
#include <tvb/handler.h>
#include <websocket_server.h>
#include <websocketpp/config/asio_no_tls.hpp>

using Server = websocketpp::server<websocketpp::config::asio>;

using MessagePtr = Server::message_ptr;

int main() {
  using namespace insite;
  spdlog::set_level(spdlog::level::trace);
  SPDLOG_INFO("Starting Insite access node");
  ServerConfig::GetInstance().ParseConfigIfExists();
  ServerConfig::GetInstance().GenerateRequestUrls();

  crow::App<crow::CORSHandler> app;
  app.stream_threshold(std::numeric_limits<unsigned int>::max());
  app.port(ServerConfig::GetInstance().port_number_access);

  NestHttpEndpoint::RegisterRoutes(app);
  ArborHttpEndpoint::RegisterRoutes(app);
  CROW_ROUTE(app, "/version/")(Version);
  CROW_ROUTE(app, "/")(Version);

  WebsocketServer srv(ServerConfig::GetInstance().port_number_access_ws + 1);

  TvbHandler tvb_handler;
  NestHandler nest_handler;

  TVBHttpEndpoint::tvb_handler = &tvb_handler;
  TVBHttpEndpoint tvb_http_endpoint;
  srv.SetTvbHandler(&tvb_handler);
  srv.SetNestHandler(&nest_handler);
  srv.StartServer();
  tvb_handler.StartThread();
  // nest_handler.StartThread();
  // nest_handler.ws_server = &srv;
  tvb_handler.srv = &srv;

  TVBHttpEndpoint::RegisterRoutes(app);

  app.run();
  srv.StopServer();
  SPDLOG_INFO("Stopped WebsocketServer");
  tvb_handler.StopThread();
  // nest_handler.StopThread();
}
