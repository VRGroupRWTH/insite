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
  const auto parameters = web::uri::split_query(request.request_uri().query());

  web::http::http_response response(web::http::status_codes::OK);
  const auto spikes = storage_->GetSpikes();
  auto spikes_begin = spikes.begin();
  auto spikes_end = spikes.end();

  const auto from = parameters.find("from");
  const auto to = parameters.find("to");

  const auto spike_happened_before = [](const Spike& spike,
                                        std::uint64_t simulation_step) {
    return spike.simulation_step < simulation_step;
  };

  if (from != parameters.end()) {
    const auto from_number = std::stoll(from->second);
    spikes_begin = std::lower_bound(spikes.begin(), spikes.end(), from_number,
                                    spike_happened_before);
  }

  if (to != parameters.end()) {
    const auto to_number = std::stoll(to->second);
    spikes_end = std::lower_bound(spikes.begin(), spikes.end(), to_number,
                                  spike_happened_before);
  }

  const auto element_count = spikes_end - spikes_begin;
  web::json::value gids = web::json::value::array(element_count);
  web::json::value simulation_steps = web::json::value::array(element_count);

  {
    size_t index = 0;
    for (auto spike = spikes_begin; spike != spikes_end; ++spike, ++index) {
      gids[index] = spike->gid;
      simulation_steps[index] = spike->simulation_step;
    }
  }

  response.set_body(web::json::value::object(
      {{"simulation_steps", simulation_steps}, {"neuron_ids", gids}}));
  return response;
}

}  // namespace insite
