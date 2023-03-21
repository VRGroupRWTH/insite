#include "config.h"
#include "jsonStrings.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include <cstdint>
#include <nest/nestNode.h>
#include <optional>
#include <toml++/impl/json_formatter.h>
#include <unordered_set>
#include <vector>

#include "rapidjson/writer.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include "utilityFunctions.h"
#include <nest/nestSpikes.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/ranges.h>
namespace insite {

using OptionalDouble = std::optional<double>;
using OptionalUInt = std::optional<uint64_t>;

template <class T> using OptionalUSet = std::optional<std::unordered_set<T>>;

using OptionalString = std::optional<std::string>;

// Receives a rapidjson-Object and checks if it has all the necessary properties
// for nodeCollectionData
void CheckNodeCollectionDataValid(
    const rapidjson::GenericObject<false, rapidjson::Value> &n) {
  assert(n.HasMember(json_strings::kModel));
  assert(n[json_strings::kModel].IsObject());
  assert(n.HasMember(json_strings::kNodeCollectionId));
  assert(n[json_strings::kNodeCollectionId].IsInt());
  assert(n.HasMember(json_strings::kNodes));
  assert(n[json_strings::kNodes].IsObject());
}

rapidjson::Value NestGetNodes(rapidjson::MemoryPoolAllocator<> &json_alloc) {
  auto node_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_nest_urls, "/nodes");

  rapidjson::Value kernel_status_result_array(rapidjson::kArrayType);

  for (auto &node_data_set : node_data_sets) {
    rapidjson::Document current_node_data;
    current_node_data.Parse(node_data_set.text.c_str());

    assert(current_node_data.IsArray());

    for (auto &node : current_node_data.GetArray()) {
      rapidjson::Value insert(node, json_alloc);
      kernel_status_result_array.PushBack(insert, json_alloc);
    }
  }

  return kernel_status_result_array;
}

rapidjson::Value NestGetNodes(rapidjson::MemoryPoolAllocator<> &json_alloc,
                              std::unordered_set<int> &param_node_ids) {
  auto node_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_nest_urls, "/nodes", 1);

  rapidjson::Value node_results(rapidjson::kArrayType);

  for (auto &node_data_set : node_data_sets) {
    rapidjson::Document current_node_data;
    current_node_data.Parse(node_data_set.text.c_str());

    assert(current_node_data.IsArray());

    for (auto &node : current_node_data.GetArray()) {
      if (param_node_ids.find(node[json_strings::kNodeId].GetInt()) ==
          param_node_ids.end()) {
        continue;
      }

      rapidjson::Value insert(node, json_alloc);
      node_results.PushBack(insert, json_alloc);
    }
  }

  return node_results;
}

rapidjson::Value NestGetNodesV2(rapidjson::MemoryPoolAllocator<> &json_alloc,
                                std::unordered_set<int> &param_node_ids) {
  auto node_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_nest_urls, "/nodes", 2);

  rapidjson::Value node_results(rapidjson::kArrayType);
  rapidjson::Value sim_id;

  for (auto &node_data_set : node_data_sets) {
    rapidjson::Document current_node_data;
    current_node_data.Parse(node_data_set.text.c_str());
    spdlog::error(node_data_set.text.c_str());

    assert(current_node_data.IsArray());

    sim_id = current_node_data["simId"];
    for (auto &node : current_node_data["nodes"].GetArray()) {
      if (param_node_ids.find(node[json_strings::kNodeId].GetInt()) ==
          param_node_ids.end()) {
        continue;
      }

      rapidjson::Value insert(node, json_alloc);
      node_results.PushBack(insert, json_alloc);
    }
  }
  rapidjson::Value result;
  result.SetObject();
  result.AddMember("simId", sim_id, json_alloc);
  result.AddMember("nodes", node_results, json_alloc);

  return result;
}

rapidjson::Value
NestGetNodeCollections(rapidjson::MemoryPoolAllocator<> &json_alloc,
                       tl::optional<int> requested_node_collection_id) {
  auto node_collection_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_nest_urls, "/nodeCollections");

  rapidjson::Value node_collection_result_array(rapidjson::kArrayType);

  std::unordered_set<int> added_node_collections;

  for (auto &node_collection_set : node_collection_data_sets) {
    auto str = node_collection_set.text;
    rapidjson::Document node_collection_data_old;
    node_collection_data_old.Parse(str.c_str());

    assert(node_collection_data_old.IsArray());

    for (auto &node_collection : node_collection_data_old.GetArray()) {
      int node_collection_id =
          node_collection.GetObject()[json_strings::kNodeCollectionId].GetInt();

      if (requested_node_collection_id &&
          requested_node_collection_id != node_collection_id) {
        continue;
      }

      auto it_node_collection = added_node_collections.find(node_collection_id);
      if (it_node_collection == added_node_collections.end()) {
        rapidjson::Value insert(node_collection, json_alloc);
        node_collection_result_array.PushBack(insert, json_alloc);

        added_node_collections.insert(node_collection_id);
      }
    }
  }

  return node_collection_result_array;
}

rapidjson::Value
NestGetNodeCollectionsV2(rapidjson::MemoryPoolAllocator<> &json_alloc,
                         tl::optional<int> requested_node_collection_id) {
  auto node_collection_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_nest_urls, "/nodeCollections", 2);

  rapidjson::Value node_collection_result_array(rapidjson::kArrayType);
  rapidjson::Value sim_id;

  std::unordered_set<int> added_node_collections;

  for (auto &node_collection_set : node_collection_data_sets) {
    auto str = node_collection_set.text;
    rapidjson::Document node_collection_data_old;
    node_collection_data_old.Parse(str.c_str());

    assert(node_collection_data_old.IsArray());

    sim_id = node_collection_data_old["simId"];
    for (auto &node_collection :
         node_collection_data_old["nodeCollections"].GetArray()) {
      int node_collection_id =
          node_collection.GetObject()[json_strings::kNodeCollectionId].GetInt();

      if (requested_node_collection_id &&
          requested_node_collection_id != node_collection_id) {
        continue;
      }

      auto it_node_collection = added_node_collections.find(node_collection_id);
      if (it_node_collection == added_node_collections.end()) {
        rapidjson::Value insert(node_collection, json_alloc);
        node_collection_result_array.PushBack(insert, json_alloc);

        added_node_collections.insert(node_collection_id);
      }
    }
  }
  rapidjson::Value result;
  result.SetObject();

  result.AddMember("simId", sim_id, json_alloc);
  result.AddMember("nodeCollections", node_collection_result_array, json_alloc);

  return result;
}

//
// #################### ENDPOINT DEFINITIONS ####################
//

crow::response NodeCollections(int api_version,
                               tl::optional<int> requested_node_collection_id) {
  rapidjson::Document result_doc;
  rapidjson::MemoryPoolAllocator<> json_alloc;
  rapidjson::Value node_collection_result_array;
  if (api_version == 1) {
    node_collection_result_array =
        NestGetNodeCollections(json_alloc, requested_node_collection_id);
  } else if (api_version == 2) {
    node_collection_result_array =
        NestGetNodeCollectionsV2(json_alloc, requested_node_collection_id);
  }

  result_doc.SetArray() = node_collection_result_array.GetArray();

  return {DocumentToString(result_doc)};
}

crow::response SpikesByNodeCollectionId(const crow::request &req,
                                        int api_version,
                                        int requested_node_collection_id) {
  SpikeParameter params(req.url_params);
  params.node_collection_id = requested_node_collection_id;

  return {NestGetSpikes(params, api_version)};
}

crow::response Nodes(int api_version) {
  rapidjson::MemoryPoolAllocator<> json_alloc;

  rapidjson::Value nodes_result_array = NestGetNodes(json_alloc);

  auto node_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_nest_urls, "/nodes", api_version);

  rapidjson::Value kernel_status_result_array(rapidjson::kArrayType);
  rapidjson::Value sim_id;

  for (auto &node_data_set : node_data_sets) {
    rapidjson::Document current_node_data;
    current_node_data.Parse(node_data_set.text.c_str());

    assert(current_node_data.IsArray());

    if (api_version == 1) {
      for (auto &node : current_node_data.GetArray()) {
        rapidjson::Value insert(node, json_alloc);
        kernel_status_result_array.PushBack(insert, json_alloc);
      }
    } else if (api_version == 2) {
      sim_id = current_node_data["simId"];

      for (auto &node : current_node_data["nodes"].GetArray()) {
        rapidjson::Value insert(node, json_alloc);
        kernel_status_result_array.PushBack(insert, json_alloc);
      }
    }
  }

  rapidjson::Document result_doc;
  if (api_version == 1) {
    result_doc.SetArray() = kernel_status_result_array;
  } else if (api_version == 2) {
    result_doc.SetObject();
    result_doc.AddMember("simId", sim_id, result_doc.GetAllocator());
    result_doc.AddMember("nodes", kernel_status_result_array,
                         result_doc.GetAllocator());
  }

  return {DocumentToString(result_doc)};
}

crow::response NodesById(int api_version, int node_id) {
  std::unordered_set<int> param_node_ids;
  param_node_ids.insert(node_id);
  rapidjson::MemoryPoolAllocator<> json_alloc;
  rapidjson::Value nodes_result_array;

  if (api_version == 1) {
    nodes_result_array = NestGetNodes(json_alloc, param_node_ids);
  } else if (api_version == 2) {
    nodes_result_array = NestGetNodesV2(json_alloc, param_node_ids);
  }

  rapidjson::Document result_doc;
  if (api_version == 1) {
    result_doc.SetArray() = nodes_result_array;

  } else if (api_version == 2) {

    result_doc.SetObject() = nodes_result_array;
  }
  return {DocumentToString(result_doc)};
}

rapidjson::Value GetNodeIds(rapidjson::MemoryPoolAllocator<> &json_alloc) {

  auto node_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_nest_urls, "/nodes");

  rapidjson::Value node_array(rapidjson::kArrayType);

  for (auto &node_data_set : node_data_sets) {
    rapidjson::Document current_node_data;
    current_node_data.Parse(node_data_set.text.c_str());
    spdlog::error(node_data_set.text.c_str());

    assert(current_node_data.IsArray());

    for (auto &node : current_node_data.GetArray()) {
      rapidjson::Value insert(node.GetObject()[json_strings::kNodeId],
                              json_alloc);
      node_array.PushBack(node.GetObject()[json_strings::kNodeId], json_alloc);
    }
  }
  return node_array;
}

rapidjson::Value GetNodeIdsV2(rapidjson::MemoryPoolAllocator<> &json_alloc) {

  auto node_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_nest_urls, "/nodes");

  rapidjson::Value node_array(rapidjson::kArrayType);
  rapidjson::Value sim_id;

  for (auto &node_data_set : node_data_sets) {
    rapidjson::Document current_node_data;
    current_node_data.Parse(node_data_set.text.c_str());
    spdlog::error(node_data_set.text.c_str());

    sim_id = current_node_data["simId"];

    assert(current_node_data.IsArray());

    for (auto &node : current_node_data["nodes"].GetArray()) {
      rapidjson::Value insert(node.GetObject()[json_strings::kNodeId],
                              json_alloc);
      node_array.PushBack(node.GetObject()[json_strings::kNodeIds], json_alloc);
    }
  }

  rapidjson::Value result(rapidjson::kObjectType);
  result.AddMember("simId", sim_id, json_alloc);
  result.AddMember("nodes", node_array, json_alloc);

  return result;
}

crow::response NodeIdEndpoint(int api_version) {

  rapidjson::MemoryPoolAllocator<> json_alloc;
  rapidjson::Document node_result;
  if (api_version == 1) {
    node_result.SetArray() = GetNodeIds(json_alloc);
  } else if (api_version == 2) {
    node_result.SetObject() = GetNodeIdsV2(json_alloc);
  }

  return {DocumentToString(node_result)};
}

} // namespace insite
