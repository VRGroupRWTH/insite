#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <cpprest/http_listener.h>
#include <string>
#include <memory>
#include <regex>

namespace insite {

class DataStorage;

class HttpServer {
 public:
  HttpServer(web::http::uri address, DataStorage* storage);
  void SimulationHasEnded(double end_time);
  void ClearSimulationHasEnded();
 private:
  web::http::experimental::listener::http_listener http_listener_;
  DataStorage* storage_;
  double simulation_has_ended_ = -1;  
  web::http::http_response GetVersion(const web::http::http_request& request);
  web::http::http_response GetKernelStatus(const web::http::http_request& request);
  web::http::http_response GetCollections(const web::http::http_request& request);
  web::http::http_response GetNodes(const web::http::http_request& request);
  web::http::http_response GetSpikeDetectors(const web::http::http_request& request);
  web::http::http_response GetSpikeRecorders(const web::http::http_request& request);
  web::http::http_response GetMultimeters(const web::http::http_request& request);

  web::http::http_response GetCurrentSimulationTime(const web::http::http_request& request);
  web::http::http_response GetSpikes(const web::http::http_request& request);
  web::http::http_response GetMultimeterMeasurement(const web::http::http_request& request);

  struct Error {
    std::string code;
    std::string message;

    web::json::value Serialize() const;
  };
  static web::http::http_response CreateErrorResponse(web::http::status_code status_code, const Error& error);
  std::vector<std::uint64_t> CommaListToUintVector(std::string input, std::regex regex = std::regex("((\\%2C)+)"));
};

}  // namespace insite

#endif  // HTTP_SERVER_HPP
