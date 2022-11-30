#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

// #include <cpprest/http_listener.h>
#include <future>
#include <string>
#include <memory>
#include <regex>
#include <crow/app.h>
#include <crow/http_request.h>
#include <crow/http_response.h>
#include "spdlog/spdlog.h"

namespace insite {

class DataStorage;

class HttpServer {
 public:
  // HttpServer(web::http::uri address, DataStorage* storage);
  HttpServer(std::string address, DataStorage* storage);
  void SimulationHasEnded(double end_time);
  void ClearSimulationHasEnded();
 private:
  // web::http::experimental::listener::http_listener http_listener_;
  crow::SimpleApp app;
  std::future<void> crow_server;
  DataStorage* storage_;
  double simulation_has_ended_ = -1;  

  
  // web::http::http_response GetVersion(const web::http::http_request& request);
  crow::response GetVersion(const crow::request& request);
  // web::http::http_response GetKernelStatus(const web::http::http_request& request);
  crow::response GetKernelStatus(const crow::request& request);
  // web::http::http_response GetCollections(const web::http::http_request& request);
  crow::response GetCollections(const crow::request& request);
  // web::http::http_response GetNodes(const web::http::http_request& request);
  crow::response GetNodes( const crow::request& request);
  // web::http::http_response GetSpikeDetectors(const web::http::http_request& request);
  // web::http::http_response GetSpikeRecorders(const web::http::http_request& request);
  crow::response GetSpikeRecorders(const crow::request& request);
  // web::http::http_response GetMultimeters(const web::http::http_request& request);
  crow::response GetMultimeters(const crow::request& request);

  // web::http::http_response GetCurrentSimulationTime(const web::http::http_request& request);
  crow::response GetCurrentSimulationTime(const crow::request& request);
  // web::http::http_response GetSpikes(const web::http::http_request& request);
  crow::response GetSpikes(const crow::request& request);
  crow::response GetSpikesFB(const crow::request& request);
  crow::response GetMultimeterMeasurement(const crow::request& request);
  // web::http::http_response GetMultimeterMeasurement(const web::http::http_request& request);

  struct Error {
    std::string code;
    std::string message;

    // web::json::value Serialize() const;
  };
  // static web::http::http_response CreateErrorResponse(web::http::status_code status_code, const Error& error);
  std::vector<std::uint64_t> CommaListToUintVector(std::string input, std::regex regex = std::regex("((\\%2C|,)+)"));
};


template <typename T>
T ConvertStringToType(const char *s);

template <>
inline bool ConvertStringToType<bool>(const char *s)
{
  return std::stoi(s);
}

template <>
inline uint64_t ConvertStringToType(const char *s)
{
  return std::stoull(s);
}

template <>
inline double ConvertStringToType(const char *s)
{
  return std::stod(s);
}

template <>
inline std::string ConvertStringToType(const char *s)
{
  return s;
}

template <typename T>
T GetParamValueOrDefault(const crow::query_string &query_string, const char *name, T default_value)
{
  T result;

  if (query_string.get(name) != nullptr)
  {
    result = ConvertStringToType<T>(query_string.get(name));
    spdlog::info("{}: {}", name, result);
    return result;
  }
  else
  {
    spdlog::info("{}: no value", name);
  }

  return default_value;
}

}  // namespace insite

#endif  // HTTP_SERVER_HPP
