#include "http_server.hpp"
#include "data_storage.hpp"

namespace insite {

HttpServer::HttpServer(web::http::uri address, DataStorage* storage)
    : http_listener_{address}, storage_(storage) {
  http_listener_.support([this](web::http::http_request request) {
    if (request.method() == "GET" &&
        request.relative_uri().path() == "/spikes") {
      request.reply(GetSpikes(request));
    } else if (request.method() == "GET" &&
        request.relative_uri().path() == "/multimeter_measurement") {
      request.reply(GetMultimeterMeasurement(request));
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
                                        double simulation_time) {
    return spike.simulation_time < simulation_time;
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
  web::json::value simulation_times = web::json::value::array(element_count);

  {
    size_t index = 0;
    for (auto spike = spikes_begin; spike != spikes_end; ++spike, ++index) {
      gids[index] = spike->gid;
      simulation_times[index] = spike->simulation_time;
    }
  }

  response.set_body(web::json::value::object(
      {{"simulation_times", simulation_times}, {"gids", gids}}));
  return response;
}

web::http::http_response HttpServer::GetMultimeterMeasurement(
    const web::http::http_request& request) {
  web::http::http_response response(web::http::status_codes::OK);
  web::json::value body = web::json::value::object();
  
  const auto parameters = web::uri::split_query(request.request_uri().query());
  const auto parameter_multimeter_id = parameters.find("multimeter_id");
  const auto parameter_attribute = parameters.find("attribute");
  const auto parameter_from = parameters.find("from");
  const auto parameter_to = parameters.find("to");
  const auto parameter_gids = parameters.find("gids");
  const auto parameter_offset = parameters.find("offset");
  const auto parameter_limit = parameters.find("limit");

  const auto multimeter_id = std::stoll(parameter_multimeter_id->second);
  const auto attribute = parameter_attribute->second;
  const auto from = parameter_from != parameters.end() 
    ? std::stoll(parameter_from->second) 
    : 0ull;
  const auto to = parameter_to != parameters.end() 
    ? std::stoll(parameter_to->second) 
    : 0ull;
  const auto filter_gids = parameter_gids != parameters.end() 
    ? std::vector<std::size_t>(
        parameter_gids->second.begin(), 
        parameter_gids->second.end()) 
    : std::vector<std::size_t>();
  const auto offset = parameter_offset != parameters.end() 
    ? std::stoll(parameter_offset->second) 
    : 0ull;
  const auto limit = parameter_limit != parameters.end() 
    ? std::stoll(parameter_limit->second) 
    : 0ull;

  const auto measurements = storage_->GetMultimeterMeasurements();
  if (measurements.find(multimeter_id) != measurements.end() &&
      measurements.at(multimeter_id).find(attribute) 
      != measurements.at(multimeter_id).end()) {
    auto& measurement = measurements.at(multimeter_id).at(attribute);
    auto& simulation_times = measurement.simulation_times;
    auto& gids = measurement.gids; // TODO: Shouldn't hide.
    auto& values = measurement.values;

    auto simulation_times_begin = from == 0 ? simulation_times.begin() 
      : std::lower_bound(simulation_times.begin(), simulation_times.end(), from);
    auto simulation_times_end = from == 0 ? simulation_times.end() 
      : std::lower_bound(simulation_times.begin(), simulation_times.end(), to);
    auto simulation_times_subset = std::vector<web::json::value>(
      simulation_times_begin, simulation_times_end);

    // TODO: Filter measurement by from/to/filter_gids/offset/limit.

    body["simulation_times"] = web::json::value::array(simulation_times_subset);
    //body["gids"] = web::json::value::array(gids_subset);
    //body["values"] = web::json::value::array(values_subset);
  }

  response.set_body(body);
  return response;
}
}  // namespace insite
