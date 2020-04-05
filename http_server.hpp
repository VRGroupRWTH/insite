#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <cpprest/http_listener.h>
#include <string>
#include <pqxx/pqxx>

namespace insite {

class DataStorage;

class HttpServer {
 public:
  HttpServer(web::http::uri address, DataStorage* storage, pqxx::connection* database_connection);

 private:
  web::http::experimental::listener::http_listener http_listener_;
  DataStorage* storage_;
  pqxx::connection* database_connection_;

  web::http::http_response GetCurrentSimulationTime(const web::http::http_request& request);

  web::http::http_response GetSpikes(const web::http::http_request& request);
  
  web::http::http_response GetMultimeterMeasurement(
    const web::http::http_request& request);
};

}  // namespace insite

#endif  // HTTP_SERVER_HPP
