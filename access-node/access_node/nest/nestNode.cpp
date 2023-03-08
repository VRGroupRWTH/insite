#include <nest/nestNode.h>
#include <cstdint>
#include <optional>
#include <unordered_set>
#include <vector>
#include "config.h"
#include "rapidjson/stringbuffer.h"

#include <nest/nestSpikes.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/ranges.h>
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include "utilityFunctions.h"
namespace insite {

using OptionalDouble = std::optional<double>;
using OptionalUInt = std::optional<uint64_t>;

template <class T>
using OptionalUSet = std::optional<std::unordered_set<T>>;

using OptionalString = std::optional<std::string>;

// Receives a rapidjson-Object and checks if it has all the necessary properties
// for nodeCollectionData
void CheckNodeCollectionDataValid(
    const rapidjson::GenericObject<false, rapidjson::Value>& n) {
  assert(n.HasMember(json_strings::kModel));
  assert(n[json_strings::kModel].IsObject());
  assert(n.HasMember(json_strings::kNodeCollectionId));
  assert(n[json_strings::kNodeCollectionId].IsInt());
  assert(n.HasMember(json_strings::kNodes));
  assert(n[json_strings::kNodes].IsObject());
}

// Get all kernelStatuses from the nest-server
rapidjson::Value NestGetNodes(rapidjson::MemoryPoolAllocator<>& json_alloc) {
  // get request results back and store in array
  auto node_data_sets =
      GetAccessNodeRequests(ServerConfig::GetInstance().request_urls, "/nodes");

  // create array to store the different kernel-data sets
  rapidjson::Value kernel_status_result_array(rapidjson::kArrayType);

  // loop through all kernelStatus-data-sets and create a new object for each
  for (auto& node_data_set : node_data_sets) {
    // create rapidjson-document for current kernelStatus-data-set and parse
    // data into it
    rapidjson::Document current_node_data;
    current_node_data.Parse(node_data_set.text.c_str());

    assert(current_node_data.IsArray());

    for (auto& node : current_node_data.GetArray()) {
      // insert created document into result array
      rapidjson::Value insert(node, json_alloc);
      kernel_status_result_array.PushBack(insert, json_alloc);
    }
  }

  return kernel_status_result_array;
}

// Get all kernelStatuses from the nest-server
rapidjson::Value NestGetNodes(rapidjson::MemoryPoolAllocator<>& json_alloc,
                              std::unordered_set<int>& param_node_ids) {
  // get request results back and store in array
  auto node_data_sets =
      GetAccessNodeRequests(ServerConfig::GetInstance().request_urls, "/nodes");

  // create array to store the different kernel-data sets
  rapidjson::Value kernel_status_result_array(rapidjson::kArrayType);

  // loop through all kernelStatus-data-sets and create a new object for each
  for (auto& node_data_set : node_data_sets) {
    // create rapidjson-document for current kernelStatus-data-set and parse
    // data into it
    rapidjson::Document current_node_data;
    current_node_data.Parse(node_data_set.text.c_str());

    assert(current_node_data.IsArray());

    for (auto& node : current_node_data.GetArray()) {
      if (param_node_ids.find(node[json_strings::kNodeId].GetInt()) ==
          param_node_ids.end()) {
        continue;
      }

      // insert created document into result array
      rapidjson::Value insert(node, json_alloc);
      kernel_status_result_array.PushBack(insert, json_alloc);
    }
  }

  return kernel_status_result_array;
}

// Get all nest nodeCollections from the nest-server
rapidjson::Value NestGetNodeCollections(
    rapidjson::MemoryPoolAllocator<>& json_alloc) {
  // get request results and store in array
  auto node_collection_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_urls, "/nodeCollections");

  // create array to store the different node-collection-data-sets sets
  rapidjson::Value node_collection_result_array(rapidjson::kArrayType);

  std::unordered_set<int> added_node_collections;

  // loop through all nodeCollection-data-sets and add the multimeters to the
  // result array
  for (auto& node_collection_set : node_collection_data_sets) {
    // create rapidjson-document for current nodeCollection-data-set and parse
    // data into it
    auto str = node_collection_set.text;
    rapidjson::Document node_collection_data_old;
    node_collection_data_old.Parse(str.c_str());

    assert(node_collection_data_old.IsArray());

    // go through all nodeCollections stored in the current data set
    for (auto& node_collection : node_collection_data_old.GetArray()) {
      // store nodeColectionId of current collection
      int node_collection_id =
          node_collection.GetObject()[json_strings::kNodeCollectionId].GetInt();

      // if nodeCollection is not in data set yet, add it
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

//
// #################### ENDPOINT DEFINITIONS ####################
//

crow::response NodeCollections() {
  // create empty rapidjson-document to store later results
  rapidjson::Document result_doc;
  result_doc.SetArray();
  rapidjson::MemoryPoolAllocator<> json_alloc;
  rapidjson::Value node_collection_result_array =
      NestGetNodeCollections(json_alloc);

  // set result_doc to result_array
  result_doc.GetArray() = node_collection_result_array.GetArray();

  return {DocumentToString(result_doc)};
}

crow::response NodesByCollectionId(int requested_node_collection_id) {
  rapidjson::MemoryPoolAllocator<> json_alloc;
  // get request results and store in array
  auto node_collection_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_urls, "/nodeCollections");

  // create array to store the different nodeIds
  std::unordered_set<int> param_nodes;

  // create bool to check if collection has been found
  bool found_collection = false;

  // loop through all nodeCollection-data-sets and add the multimeters to the
  // result array
  for (auto& node_collection_set : node_collection_data_sets) {
    if (found_collection) {
      break;
    }

    // create rapidjson-document for current nodeCollection-data-set and parse
    // data into it
    rapidjson::Document node_collection_data_old;
    node_collection_data_old.Parse(node_collection_set.text.c_str());

    assert(node_collection_data_old.IsArray());

    // go through all nodeCollections stored in the current data set
    for (auto& node_collection : node_collection_data_old.GetArray()) {
      // store nodeColectionId of current collection
      int node_collection_id =
          node_collection.GetObject()[json_strings::kNodeCollectionId].GetInt();

      // check if collection with given id has been found
      if (node_collection_id == requested_node_collection_id) {
        rapidjson::GenericObject<false, rapidjson::Value>
            return_node_collection =
                node_collection.GetObject()["nodes"].GetObject();

        // retrieve and store node information
        int first_id = return_node_collection[json_strings::kFirstId].GetInt();
        int last_id = return_node_collection[json_strings::kLastId].GetInt();
        int count = return_node_collection[json_strings::kCount].GetInt();

        // create array antry for every node in the collection
        for (int i = first_id; i <= last_id; ++i) {
          param_nodes.insert(i);
        }

        found_collection = true;
      }
    }
  }

  // create empty rapidjson-document to store later results
  rapidjson::Document result_doc;
  rapidjson::Value result_nodes = NestGetNodes(json_alloc, param_nodes);
  result_doc.SetArray();

  // set result array as the result document
  result_doc.GetArray() = result_nodes.GetArray();

  // create output string out of result document
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  result_doc.Accept(writer);

  return {buffer.GetString()};
}

crow::response NodeIdsByNodeCollectionId(int requested_node_collecton_id) {
  rapidjson::MemoryPoolAllocator<> json_alloc;
  // get request results and store in array
  auto node_collection_data_sets = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_urls, "/nodeCollections");

  // create array to store the different nodeIds
  rapidjson::Value nodes_result_array(rapidjson::kArrayType);

  // create bool to check if collection has been found
  bool found_collection = false;

  // loop through all nodeCollection-data-sets and add the multimeters to the
  // result array
  for (auto& node_collection_set : node_collection_data_sets) {
    if (found_collection) {
      break;
    }

    // create rapidjson-document for current nodeCollection-data-set and parse
    // data into it
    rapidjson::Document node_collection_data_old;
    node_collection_data_old.Parse(node_collection_set.text.c_str());

    assert(node_collection_data_old.IsArray());

    // go through all nodeCollections stored in the current data set
    for (auto& node_collection : node_collection_data_old.GetArray()) {
      // store nodeColectionId of current collection
      int node_collection_id =
          node_collection.GetObject()[json_strings::kNodeCollectionId].GetInt();

      // check if collection with given id has been found
      if (node_collection_id == requested_node_collecton_id) {
        rapidjson::GenericObject<false, rapidjson::Value>
            return_node_collection =
                node_collection.GetObject()["nodes"].GetObject();

        // retrieve and store node information
        int first_id = return_node_collection[json_strings::kFirstId].GetInt();
        int last_id = return_node_collection[json_strings::kLastId].GetInt();
        int count = return_node_collection[json_strings::kCount].GetInt();

        // create array antry for every node in the collection
        for (int i = first_id; i <= last_id; ++i) {
          nodes_result_array.PushBack(i, json_alloc);
        }

        found_collection = true;
      }
    }
  }

  // create empty rapidjson-document to store later results
  rapidjson::Document result_doc;
  result_doc.SetArray();

  // set result array as the result document
  result_doc.GetArray() = nodes_result_array.GetArray();

  // create output string out of result document
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  result_doc.Accept(writer);

  return {buffer.GetString()};
}

crow::response SpikesByNodeCollectionId(const crow::request& req,
                                        int requested_node_collection_id) {
  SpikeParameter params(req.url_params);

  rapidjson::MemoryPoolAllocator<> json_alloc;
  rapidjson::Value node_collection_result_array =
      NestGetNodeCollections(json_alloc);
  assert(node_collection_result_array.IsArray());

  std::unordered_set<int> response_node_ids;
  for (auto& current_node_collection :
       node_collection_result_array.GetArray()) {
    CheckNodeCollectionDataValid(current_node_collection.GetObject());

    if (current_node_collection.GetObject()[json_strings::kNodeCollectionId] ==
        requested_node_collection_id) {
      int start = current_node_collection.GetObject()[json_strings::kNodes]
                      .GetObject()[json_strings::kFirstId]
                      .GetInt();
      int last = current_node_collection.GetObject()[json_strings::kNodes]
                     .GetObject()[json_strings::kLastId]
                     .GetInt();

      for (int i = start; i <= last; ++i) {
        response_node_ids.insert(i);
      }
      break;
    }
  }

  std::unordered_set<uint64_t> query_node_ids;
  std::vector<uint64_t> query_node_vec;

  if (params.node_gids && params.node_gids.value().empty()) {
    std::copy(response_node_ids.begin(), response_node_ids.end(),
              std::inserter(query_node_ids, query_node_ids.begin()));
  } else if (params.node_gids) {
    std::set_intersection(
        params.node_gids.value().begin(), params.node_gids.value().end(),
        response_node_ids.begin(), response_node_ids.end(),
        std::inserter(query_node_ids, query_node_ids.begin()));
  }
  query_node_vec.insert(query_node_vec.end(), query_node_ids.begin(),
                        query_node_ids.end());
  params.node_gids = query_node_vec;
  SPDLOG_INFO("filtered ids: {}", query_node_vec);

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  if (query_node_vec.empty()) {
    SpikeContainer empty;
    empty.SerializeToJson(writer, false);
    return {buffer.GetString()};
  }

  SpikeContainer spikes = std::move(NestGetSpikes(params));
  spikes.SerializeToJson(writer, true);

  return {buffer.GetString()};
}

crow::response Nodes() {
  // create empty rapidjson-document to store later results
  rapidjson::Document result_doc;
  result_doc.SetArray();

  rapidjson::MemoryPoolAllocator<> json_alloc;
  rapidjson::Value nodes_result_array = NestGetNodes(json_alloc);

  // awr result array as result document
  result_doc.GetArray() = nodes_result_array.GetArray();

  return {DocumentToString(result_doc)};
}

crow::response NodesById(int node_id) {
  std::unordered_set<int> param_node_ids;
  param_node_ids.insert(node_id);

  rapidjson::MemoryPoolAllocator<> json_alloc;
  rapidjson::Value nodes_result_array =
      NestGetNodes(json_alloc, param_node_ids);

  // create empty rapidjson-document to store later results
  rapidjson::Document result_doc;
  result_doc.SetArray();

  // awr result array as result document
  result_doc.GetArray() = nodes_result_array.GetArray();
  return {DocumentToString(result_doc)};
}

crow::response NodeIds() {
  rapidjson::MemoryPoolAllocator<> json_alloc;
  rapidjson::Value nodes_result_array = NestGetNodes(json_alloc);
  rapidjson::Value node_ids_result_array(rapidjson::kArrayType);

  for (const auto& node_result : nodes_result_array.GetArray()) {
    uint64_t node_id =
        node_result.GetObject()[json_strings::kNodeId].GetUint64();
    node_ids_result_array.GetArray().PushBack(node_id, json_alloc);
  }

  // create empty rapidjson-document to store later results
  rapidjson::Document result_doc;
  result_doc.SetArray();

  // awr result array as result document
  result_doc.GetArray() = node_ids_result_array.GetArray();
  return {DocumentToString(result_doc)};
}
}  // namespace insite
