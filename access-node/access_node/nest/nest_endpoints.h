#pragma once

#include <nest/nestKernelStatus.h>
#include <nest/nestMultimeter.h>
#include <nest/nestNode.h>
#include <nest/nestSpikeRecorders.h>
#include <nest/nestSpikes.h>
#include <nest/nestVersion.h>
#include "crow/app.h"

namespace insite {
class NestHttpEndpoint {
 public:
  static void RegisterRoutes(crow::SimpleApp& app) {
    // clang-format off
  CROW_ROUTE(app, "/nest/kernelStatus")(KernelStatus);
  CROW_ROUTE(app, "/nest/multimeters")(MultimeterResponse);
  CROW_ROUTE(app, "/nest/multimeters/<int>")(MultimeterByIdResponse);
  // CROW_ROUTE(app, "/nest/multimeters/<int>/attributes/<string>")(MultimeterAttributes);
  CROW_ROUTE(app, "/nest/multimeters/<int>/attributes/<string>")(MultimeterAttributesResponse);
  CROW_ROUTE(app, "/nest/nodeCollections")(NodeCollections);
  CROW_ROUTE(app, "/nest/nodeCollections/<int>/nodes")(NodesByCollectionId);
  CROW_ROUTE(app, "/nest/nodeCollections/<int>/nodes/ids")(NodeIdsByNodeCollectionId);
  CROW_ROUTE(app, "/nest/nodeCollections/<int>/spikes")(SpikesByNodeCollectionId);
  CROW_ROUTE(app, "/nest/nodes")(Nodes);
  CROW_ROUTE(app, "/nest/nodes/ids")(NodeIds);
  CROW_ROUTE(app, "/nest/nodes/<int>")(NodesById);
  CROW_ROUTE(app, "/nest/simulationTimeInfo")(SimulationTimeInfo);
  CROW_ROUTE(app, "/nest/spikerecorders")(SpikeRecorders);
  CROW_ROUTE(app, "/nest/spikerecorders/<int>/spikes")(SpikesBySpikeRecorderId);
  CROW_ROUTE(app, "/nest/spikes")(Spikes);
  CROW_ROUTE(app, "/nest/spikesfb")(Spikesfb);
    // clang-format on
  }
};  // namespace nest
}  // namespace insite
