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
  static void RegisterRoutes(crow::App<crow::CORSHandler> &app) {
    // clang-format off
  CROW_ROUTE(app, "/arbor/cell_infos/")(arbor::GetCellInfos);
  CROW_ROUTE(app, "/arbor/cells/")(arbor::GetCells);
  CROW_ROUTE(app, "/arbor/probe_data/")(arbor::GetProbeData);
  CROW_ROUTE(app, "/arbor/spikes/")(arbor::GetSpikes);
  CROW_ROUTE(app, "/arbor/probes/")(arbor::GetProbes);
  // CROW_ROUTE(app_, "/version", [](const crow::request& req);
    // clang-format on
  }
}; // namespace nest
} // namespace insite
