#include "http_server.hpp"
#include "data_storage.hpp"

namespace insite {

HttpServer::HttpServer(web::http::uri address, DataStorage* storage)
    : http_listener_{address}, storage_(storage) {
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
  const auto spikes = storage_->GetSpikes();
  web::json::value gids = web::json::value::array(spikes.size());
  web::json::value simulation_steps = web::json::value::array(spikes.size());
  for (size_t i = 0; i < spikes.size(); ++i) {
    gids[i] = spikes[i].gid;
    simulation_steps[i] = spikes[i].simulation_step;
  }
  response.set_body(web::json::value::object(
      {{"simulation_steps", simulation_steps}, {"neuron_ids", gids}}));
  return response;
}

}  // namespace insite
