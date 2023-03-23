#pragma once
#include <crow.h>
#include <rapidjson/document.h>

namespace insite {

// Get all kernelStatuses from the nest-server
rapidjson::Value NestGetKernelStatuses(rapidjson::MemoryPoolAllocator<>& json_alloc);
rapidjson::Value NestGetKernelStatusesV2(rapidjson::MemoryPoolAllocator<>& json_alloc);

crow::response KernelStatus(int api_version);

}  // namespace insite
