#include "config.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include <cpr/cpr.h>
#include <iterator>
#include <spdlog/spdlog.h>
#include <unordered_set>
#include <vector>

#include "utilityFunctions.h"
namespace insite {
// Get all kernelStatuses from the nest-server
rapidjson::Value
NestGetKernelStatuses(int api_version,
                      rapidjson::MemoryPoolAllocator<> &json_alloc) {
  // get request results back and store in array

  std::string endpoint;
  switch (api_version) {
  case 1:
    endpoint = "/kernelStatus";
    break;
  case 2:
    endpoint = "/v2/kernelStatus";
    break;
  }
  CprResponseVec kernel_statuses = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_nest_urls, endpoint);

  // create array to store the different kernel-data sets
  rapidjson::Value kernel_status_result_array(rapidjson::kArrayType);

  // loop through all kernelStatus-data-sets and create a new object for each
  for (cpr::Response &kernel_status : kernel_statuses) {
    // create rapidjson-document for current kernelStatus-data-set and parse
    // data into it
    rapidjson::Document kernel_data_old;
    kernel_data_old.Parse(kernel_status.text.c_str());

    assert(kernel_data_old.IsObject());

    rapidjson::Document kernel_data_new;
    kernel_data_new.SetObject();

    // copy all members from the old data-set to the new one
    for (auto &member : kernel_data_old.GetObject()) {
      rapidjson::Value key(member.name.GetString(), json_alloc);
      // rapidjson::Value key(member.name.GetString(), member.name.Size());
      kernel_data_new.AddMember(key, member.value, json_alloc);
    }

    // insert created document into result array
    rapidjson::Value insert(kernel_data_new, json_alloc);
    kernel_status_result_array.PushBack(insert, json_alloc);
  }

  return kernel_status_result_array;
}

//
// #################### ENDPOINT DEFINITIONS ####################
//

crow::response KernelStatus(int api_version) {
  rapidjson::MemoryPoolAllocator<> json_alloc;
  rapidjson::Value kernel_status_result_array =
      NestGetKernelStatuses(api_version, json_alloc);

  // create empty rapidjson-document to store later results
  rapidjson::Document result_doc;
  result_doc.SetArray() = kernel_status_result_array.GetArray();

  return {DocumentToString(result_doc)};
}
} // namespace insite
