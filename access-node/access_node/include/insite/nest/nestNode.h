#pragma once

#include <crow.h>
#include <rapidjson/document.h>
#include <unordered_set>

namespace insite {

void CheckNodeCollectionDataValid(
    const rapidjson::GenericObject<false, rapidjson::Value>& n);

rapidjson::Value NestGetNodes(rapidjson::MemoryPoolAllocator<>& json_alloc);

rapidjson::Value NestGetNodes(rapidjson::MemoryPoolAllocator<>& json_alloc,
                              std::unordered_set<int>& param_node_ids);

rapidjson::Value NestGetNodeCollections(
    rapidjson::MemoryPoolAllocator<>& json_alloc);

crow::response NodeCollections();

crow::response NodesByCollectionId(int requested_node_collection_id);

crow::response NodeIdsByNodeCollectionId(int requested_node_collection_id);

crow::response SpikesByNodeCollectionId(const crow::request& req,
                                        int requested_node_collection_id);

crow::response Nodes();

crow::response NodesById(int requested_node_collection_id);

crow::response NodeIds();
}  // namespace insite
