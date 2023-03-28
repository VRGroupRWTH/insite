#pragma once

#include "arbor/cell_endpoints.h"
#include "arbor/probe_endpoints.h"
#include "arbor/spike_endpoints.h"
#include "config.h"
#include "crow/app.h"
#include "crow/middlewares/cors.h"
#include <utility_functions.h>

namespace insite {

class ArborHttpEndpoint {
public:
  static void RegisterRoutes(crow::App<crow::CORSHandler> &app);
  
}; // namespace nest

} // namespace insite
