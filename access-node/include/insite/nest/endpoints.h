#pragma once

#include <nest/kernel_status.h>
#include <nest/multimeter.h>
#include <nest/nodecollections.h>
#include <nest/spikerecorders.h>
#include <nest/spikes.h>
#include <nest/version.h>
#include "crow/app.h"
#include "crow/http_request.h"
#include "crow/middlewares/cors.h"
#include "tl/optional.hpp"

#define CROW_ROUTE_LAMBA(APP, ENDPOINT, FUNCTION) \
  CROW_ROUTE(APP, ENDPOINT)                       \
  ([this](const crow::request& request) { return FUNCTION(request); });

#define CROW_ROUTE_LAMBA2(APP, ENDPOINT, FUNCTION) \
  CROW_ROUTE(APP, ENDPOINT)                        \
  ([](const crow::request& request) FUNCTION);

namespace insite {
using ApiVersion = int;

class NestHttpEndpoint {
 public:
  static void RegisterRoutes(crow::App<crow::CORSHandler>& app);
};  // namespace nest
}  // namespace insite
