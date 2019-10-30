#include "http_server.hpp"

namespace insite {

HttpServer::HttpServer(web::http::uri address, DataStorage* storage)
    : http_listener_{address} {
  http_listener_.support([this](web::http::http_request request) {
    if (request.method() == "GET" &&
        request.relative_uri().path() == "/spikes") {
      request.reply(GetSpikes(request));
    } else {
      std::cerr << "Invalid request: " << request.to_string() << "\n";
      request.reply(web::http::status_codes::NotFound);
    }
  });

  http_listener_.open().wait();
  std::cout << "HTTP server is listening...\n";
}

web::http::http_response HttpServer::GetSpikes(
    const web::http::http_request& request) {
  web::http::http_response response(web::http::status_codes::OK);
  web::json::value response_body("Hello World!");
  response.set_body(response_body);
  return response;
}

}  // namespace insite
