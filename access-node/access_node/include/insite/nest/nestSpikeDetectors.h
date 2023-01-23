#pragma once
// #include "rapidjson/document.h"
// #include "rapidjson/stringbuffer.h"
// #include <cpr/cpr.h>
// #include <iostream>
// #include <iterator>
// #include <optional>
// #include <spdlog/spdlog.h>
// #include <unordered_set>
// #include <vector>
//
// #include "nestSpikes.h"
// #include "utilityFunctions.h"
// #include "spikedetector.h"
//
// namespace insite {
// // Receives a rapidjson-Object and checks if it has all the necessary
// properties
// // for spikeDetectorData
// void checkSpikeDetectorDataValid(
//     const rapidjson::GenericObject<false, rapidjson::Value> &r) {
//   assert(r.HasMember(jsonStrings::spikeDetectorId) &&
//          r[jsonStrings::spikeDetectorId].IsInt());
//   assert(r.HasMember(jsonStrings::nodeIds) &&
//          r[jsonStrings::nodeIds].IsArray());
// }
//
// // Combines the properties of two given spikeDetectors r1 and r2 and stores
// it
// // in r1
// void mergeSpikeDetectors(
//     const rapidjson::GenericObject<false, rapidjson::Value> &r1,
//     const rapidjson::GenericObject<false, rapidjson::Value> &r2) {
//   checkSpikeDetectorDataValid(r1);
//   checkSpikeDetectorDataValid(r2);
//
//   std::set<int> addedNodeIds;
//
//   for (auto &id : r1[jsonStrings::nodeIds].GetArray()) {
//     addedNodeIds.insert(id.GetInt());
//   }
//
//   for (auto &id : r2[jsonStrings::nodeIds].GetArray()) {
//     std::set<int>::iterator it = addedNodeIds.find(id.GetInt());
//
//     if (it == addedNodeIds.end()) {
//       addedNodeIds.insert(id.GetInt());
//     }
//   }
//
//   r1[jsonStrings::nodeIds].GetArray().Clear();
//   for (const int &id : addedNodeIds) {
//     r1[jsonStrings::nodeIds].GetArray().PushBack(id, jsonAlloc);
//   }
// }
//
//
// // Get all nest spikeDetectors from the nest-server
// rapidjson::Value nestGetSpikeDetectors() {
//   // get request results and store in array
//   std::vector<std::future<std::string>> spikeDetector_data_sets =
//       getAccessNodeRequests(request_urls, "/spikedetectors");
//
//   rapidjson::Value spikeDetector_result_array(rapidjson::kArrayType);
//
//   std::unordered_set<int> addedSpikeDetectors;
//
//   for (auto &spikeDetector_data_set : spikeDetector_data_sets) {
//     std::string s = spikeDetector_data_set.get();
//
//     rapidjson::Document spikeDetector_data_old;
//     spikeDetector_data_old.Parse(s.c_str());
//
//     assert(spikeDetector_data_old.IsArray());
//
//     for (auto &current_spikeDetector : spikeDetector_data_old.GetArray()) {
//       int current_spikeDetectorId =
//           current_spikeDetector.GetObject()[jsonStrings::spikeDetectorId]
//               .GetInt();
//
//       std::unordered_set<int>::iterator it =
//           addedSpikeDetectors.find(current_spikeDetectorId);
//       if (it == addedSpikeDetectors.end()) {
//         rapidjson::Value insert(current_spikeDetector, jsonAlloc);
//         spikeDetector_result_array.PushBack(insert, jsonAlloc);
//         addedSpikeDetectors.insert(current_spikeDetectorId);
//       } else {
//         for (auto &v : spikeDetector_result_array.GetArray()) {
//           if (v.GetObject()[jsonStrings::spikeDetectorId].GetInt() ==
//               current_spikeDetectorId) {
//             mergeSpikeDetectors(v.GetObject(),
//                                 current_spikeDetector.GetObject());
//           }
//         }
//       }
//     }
//   }
//
//   return spikeDetector_result_array;
// }
//
// // Get all spikes that belong to the given spikeDetectorId
// crow::response nestGetSpikesBySpikeDetector(
//     const uint64_t &id, const std::optional<double> &fromTime,
//     const std::optional<double> &toTime,
//     const std::unordered_set<uint64_t> &nodeIds,
//     const std::optional<uint64_t> &skip, const std::optional<uint64_t> &top,
//     const std::optional<std::string> &sort) {
//   rapidjson::Value spikeDetector_result_array = nestGetSpikeDetectors();
//
//   std::unordered_set<uint64_t> response_nodeIds;
//   for (auto &current_spikeDetector : spikeDetector_result_array.GetArray()) {
//     checkSpikeDetectorDataValid(current_spikeDetector.GetObject());
//
//     if (current_spikeDetector.GetObject()[jsonStrings::spikeDetectorId] ==
//     id) {
//       for (auto &nodeId :
//            current_spikeDetector.GetObject()[jsonStrings::nodeIds].GetArray())
//            {
//         response_nodeIds.insert(nodeId.GetInt());
//       }
//       break;
//     }
//   }
//
//   std::unordered_set<uint64_t> query_nodeIds;
//
//   if (nodeIds.size() == 0) {
//     std::copy(response_nodeIds.begin(), response_nodeIds.end(),
//               std::inserter(query_nodeIds, query_nodeIds.begin()));
//   } else {
//     std::set_intersection(nodeIds.begin(), nodeIds.end(),
//                           response_nodeIds.begin(), response_nodeIds.end(),
//                           std::inserter(query_nodeIds,
//                           query_nodeIds.begin()));
//   }
//
//     rapidjson::StringBuffer buffer;
//     rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
//     SpikeContainer spikes = std::move(nestGetSpikes(fromTime, toTime,
//     query_nodeIds, skip, top, sort)); spikes.serializeToJSON(writer);
//
//     return crow::response(buffer.GetString());
// }
//
// //
// // #################### ENDPOINT DEFINITIONS ####################
// //
//
// crow::response spikeDetectors() {
//   rapidjson::Value spikeDetector_result_array = nestGetSpikeDetectors();
//
//   // create empty rapidjson-document to store later results
//   rapidjson::Document result_doc;
//   result_doc.SetArray();
//
//   // set result array as the result document
//   result_doc.GetArray() = spikeDetector_result_array.GetArray();
//
//   return crow::response(documentToString(result_doc));
// }
//
// crow::response spikesBySpikeDetectorId(const crow::request &req, int id) {
//   std::optional<double> fromTime = getParam<double>(req,
//   jsonStrings::fromTime); std::optional<double> toTime =
//   getParam<double>(req, jsonStrings::toTime); std::unordered_set<uint64_t>
//   nodeIds =
//       getParamUSet<uint64_t>(req, jsonStrings::nodeIds);
//   std::optional<uint64_t> skip = getParam<uint64_t>(req, jsonStrings::skip);
//   std::optional<uint64_t> top = getParam<uint64_t>(req, jsonStrings::top);
//   std::optional<std::string> sort =
//       getParam<std::string>(req, jsonStrings::sort);
//
//   // std::string result = nestGetSpikesBySpikeDetector(id, fromTime, toTime,
//   // param_nodeIds, skip, top, sort);
//   rapidjson::Value spikeDetector_result_array = nestGetSpikeDetectors();
//
//   std::unordered_set<uint64_t> nodesInSpikedetector;
//   for (auto &current_spikeDetector : spikeDetector_result_array.GetArray()) {
//     checkSpikeDetectorDataValid(current_spikeDetector.GetObject());
//
//     if (current_spikeDetector.GetObject()[jsonStrings::spikeDetectorId] ==
//     id) {
//       for (auto &nodeId :
//            current_spikeDetector.GetObject()[jsonStrings::nodeIds].GetArray())
//            {
//         nodesInSpikedetector.insert(nodeId.GetInt());
//       }
//       break;
//     }
//   }
//
//   std::unordered_set<uint64_t> query_nodeIds;
//
//   if (nodeIds.empty()) {
//     query_nodeIds = std::move(nodeIds);
//   } else {
//     std::copy_if(nodeIds.begin(), nodeIds.end(),
//                  std::inserter(query_nodeIds, query_nodeIds.begin()),
//                  [nodesInSpikedetector](const int element) {
//                    return nodesInSpikedetector.count(element) > 0;
//                  });
//   }
// //
//   // std::string result =
//   nestGetSpikes(fromTime,toTime,query_nodeIds,skip,top,sort);
//   // return crow::response(result);
//   return crow::response("Error");
// }
// }
