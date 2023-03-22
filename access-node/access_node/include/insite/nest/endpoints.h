#pragma once

#include "crow/app.h"
#include "crow/http_request.h"
#include "crow/middlewares/cors.h"
#include "tl/optional.hpp"
#include <nest/kernel_status.h>
#include <nest/multimeter.h>
#include <nest/nodecollections.h>
#include <nest/spikerecorders.h>
#include <nest/spikes.h>
#include <nest/version.h>

#define CROW_ROUTE_LAMBA(APP, ENDPOINT, FUNCTION)                              \
  CROW_ROUTE(APP, ENDPOINT)                                                    \
  ([this](const crow::request &request) { return FUNCTION(request); });

#define CROW_ROUTE_LAMBA2(APP, ENDPOINT, FUNCTION)                             \
  CROW_ROUTE(APP, ENDPOINT)                                                    \
  ([](const crow::request &request) FUNCTION);

namespace insite {
using ApiVersion = int;

class NestHttpEndpoint {
public:
  static void RegisterRoutes(crow::App<crow::CORSHandler> &app) {
    // clang-format off
  CROW_ROUTE_LAMBA2(app, "/nest/kernelStatus/",{return KernelStatus(ApiVersion(1));})
  CROW_ROUTE_LAMBA2(app, "/v2/nest/kernelStatus/",{return KernelStatus(ApiVersion(2));})

  CROW_ROUTE_LAMBA2(app, "/nest/multimeters/",{return MultimeterResponse(ApiVersion(1));});
  CROW_ROUTE_LAMBA2(app, "/v2/nest/multimeters/",{return MultimeterResponse(ApiVersion(2));});

  CROW_ROUTE(app, "/nest/multimeters/<int>/")([](const crow::request& req, int multimeter_id){return MultimeterByIdResponse(ApiVersion(1), multimeter_id);});
  CROW_ROUTE(app, "/v2/nest/multimeters/<int>/")([](const crow::request& req, int multimeter_id){return MultimeterByIdResponse(ApiVersion(2), multimeter_id);});

  CROW_ROUTE(app, "/nest/multimeters/<int>/attributes/<string>/")([](const crow::request& req, int multimeter_id, const std::string& attribute_name){return MultimeterAttributesResponse(req,ApiVersion(1), multimeter_id, attribute_name);});
  CROW_ROUTE(app, "/v2/nest/multimeters/<int>/attributes/<string>/")([](const crow::request& req, int multimeter_id, const std::string& attribute_name){return MultimeterAttributesResponse(req,ApiVersion(2), multimeter_id, attribute_name);});

  CROW_ROUTE_LAMBA2(app, "/nest/nodeCollections/",{return NodeCollections(ApiVersion(1),tl::nullopt);});
  CROW_ROUTE_LAMBA2(app, "/v2/nest/nodeCollections/",{return NodeCollections(ApiVersion(2), tl::nullopt);});

  CROW_ROUTE(app, "/nest/nodeCollections/<int>/nodes/")([](const crow::request& req, int node_collection_id){return NodeCollections(ApiVersion(1), node_collection_id);});
  CROW_ROUTE(app, "/v2/nest/nodeCollections/<int>/nodes/")([](const crow::request& req, int node_collection_id){return NodeCollections(ApiVersion(2), node_collection_id);});

  CROW_ROUTE(app, "/nest/nodeCollections/<int>/spikes/")([](const crow::request& req, int node_collection_id){return SpikesByNodeCollectionId(req, ApiVersion(1), node_collection_id);});
  CROW_ROUTE(app, "/v2/nest/nodeCollections/<int>/spikes/")([](const crow::request& req, int node_collection_id){return SpikesByNodeCollectionId(req, ApiVersion(2), node_collection_id);});

  CROW_ROUTE_LAMBA2(app, "/nest/nodes/",{return Nodes(ApiVersion(1));});
  CROW_ROUTE_LAMBA2(app, "/v2/nest/nodes/",{return Nodes(ApiVersion(2));});

  CROW_ROUTE_LAMBA2(app, "/nest/nodes/ids/",{return NodeIdEndpoint(ApiVersion(1));});
  CROW_ROUTE_LAMBA2(app, "/v2/nest/nodes/ids/",{return NodeIdEndpoint(ApiVersion(2));});

  CROW_ROUTE(app, "/nest/nodes/<int>/")([](const crow::request& req, int node_collection_id){return NodesById(ApiVersion(1), node_collection_id);});
  CROW_ROUTE(app, "/v2/nest/nodes/<int>/")([](const crow::request& req, int node_collection_id){return NodesById(ApiVersion(2), node_collection_id);});

  CROW_ROUTE_LAMBA2(app, "/nest/simulationTimeInfo/",{return SimulationTimeInfo(ApiVersion(1));});
  CROW_ROUTE_LAMBA2(app, "/v2/nest/simulationTimeInfo/",{return SimulationTimeInfo(ApiVersion(2));});

  CROW_ROUTE_LAMBA2(app, "/nest/spikerecorders/",{return SpikeRecorders(ApiVersion(1));});
  CROW_ROUTE_LAMBA2(app, "/v2/nest/spikerecorders/",{return SpikeRecorders(ApiVersion(2));});
 
  CROW_ROUTE(app, "/nest/spikerecorders/<int>/spikes/")([](const crow::request& req, int spikerecorder_id){return SpikesBySpikeRecorderId(req, ApiVersion(1),spikerecorder_id);});
  CROW_ROUTE(app, "/v2/nest/spikerecorders/<int>/spikes/")([](const crow::request& req, int spikerecorder_id){return SpikesBySpikeRecorderId(req, ApiVersion(2),spikerecorder_id);});
  
  CROW_ROUTE_LAMBA2(app, "/nest/spikes/",{return NestGetSpikes(request.url_params, ApiVersion(1));});
  CROW_ROUTE_LAMBA2(app, "/v2/nest/spikes/",{return NestGetSpikes(request.url_params,ApiVersion(2));});

  // CROW_ROUTE(app, "/nest/spikesfb/")(Spikesfb);
    // clang-format on
  }
}; // namespace nest
} // namespace insite
