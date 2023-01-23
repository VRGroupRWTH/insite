#include <cpr/cpr.h>
#include <spdlog/spdlog.h>
#include <iterator>
#include <unordered_set>
#include <vector>
#include "config.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

#include "utilityFunctions.h"
namespace insite {
// Get all kernelStatuses from the nest-server
rapidjson::Value NestGetKernelStatuses() {
  rapidjson::MemoryPoolAllocator<> json_alloc;
  // get request results back and store in array
  std::vector<std::future<std::string>> kernel_statuses = GetAccessNodeRequests(
      ServerConfig::GetInstance().request_urls, "/kernelStatus");

  // create array to store the different kernel-data sets
  rapidjson::Value kernel_status_result_array(rapidjson::kArrayType);

  // loop through all kernelStatus-data-sets and create a new object for each
  for (std::future<std::string>& kernel_status : kernel_statuses) {

    // create rapidjson-document for current kernelStatus-data-set and parse
    // data into it
    rapidjson::Document kernel_data_old;
    kernel_data_old.Parse(kernel_status.get().c_str());

    assert(kernel_data_old.IsObject());

    rapidjson::Document kernel_data_new;
    kernel_data_new.SetObject();

    // copy all members from the old data-set to the new one
    for (auto& member : kernel_data_old.GetObject()) {
      rapidjson::Value key(member.name.GetString(), json_alloc);
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

crow::response KernelStatus() {
  rapidjson::Value kernel_status_result_array = NestGetKernelStatuses();

  // create empty rapidjson-document to store later results
  rapidjson::Document result_doc;
  result_doc.SetArray();

  // awr result array as result document
  result_doc.GetArray() = kernel_status_result_array.GetArray();

  return {DocumentToString(result_doc)};
}
}  // namespace insite
