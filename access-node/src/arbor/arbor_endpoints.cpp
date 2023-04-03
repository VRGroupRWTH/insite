#include <arbor/arbor_endpoints.h>

namespace insite {

void ArborHttpEndpoint::RegisterRoutes(crow::App<crow::CORSHandler>& app) {
  // clang-format off
    CROW_ROUTE(app, "/arbor/cellInfos/")(arbor::GetCellInfos);
    CROW_ROUTE(app, "/arbor/cells/")(arbor::GetCells);
    CROW_ROUTE(app, "/arbor/probeData/")(arbor::GetProbeData); CROW_ROUTE(app, "/arbor/spikes/")(arbor::GetSpikes);
    CROW_ROUTE(app, "/arbor/probes/")(arbor::GetProbes);
    // CROW_ROUTE(app_, "/version", [](const crow::request& req);
  // clang-format on
}

}  // namespace insite
