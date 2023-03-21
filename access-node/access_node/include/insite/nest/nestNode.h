#pragma once

#include "tl/optional.hpp"
#include <crow.h>
#include <rapidjson/document.h>
#include <unordered_set>

namespace insite {

void CheckNodeCollectionDataValid(
    const rapidjson::GenericObject<false, rapidjson::Value> &n);

rapidjson::Value NestGetNodes(rapidjson::MemoryPoolAllocator<> &json_alloc);

rapidjson::Value NestGetNodes(rapidjson::MemoryPoolAllocator<> &json_alloc,
                              std::unordered_set<int> &param_node_ids);

rapidjson::Value NestGetNodeCollections(
    rapidjson::MemoryPoolAllocator<> &json_alloc,
    tl::optional<int> requested_node_collection_id = tl::nullopt);

rapidjson::Value NestGetNodeCollectionsV2(
    rapidjson::MemoryPoolAllocator<> &json_alloc,
    tl::optional<int> requested_node_collection_id = tl::nullopt);

crow::response
NodeCollections(int api_version,
                tl::optional<int> requested_node_collection_id = tl::nullopt);

crow::response SpikesByNodeCollectionId(const crow::request &req,
                                        int api_version,
                                        int requested_node_collection_id);

crow::response Nodes(int api_version);

crow::response NodesById(int api_version, int node_id);

crow::response NodeIdEndpoint(int api_version);
// crow::response NodeIds();
} // namespace insite
