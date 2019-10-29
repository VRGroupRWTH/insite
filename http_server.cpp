#include "http_server.hpp"

namespace insite {

HttpServer::HttpServer(web::http::uri address, DataStorage* storage)
    : http_listener_{address} {
  http_listener_.support([](web::http::http_request request) {
    // std::cout << request.method() << " " << request.request_uri().path()
    //           << "\n";
    // web::http::http_response response(200);
    // web::json::value response_body("Hello World!");
    // response.set_body(response_body);
    // request.reply(response);
  });

  http_listener_.open();
  std::cout << "HTTP server is listening...\n";
}

}  // namespace insite
