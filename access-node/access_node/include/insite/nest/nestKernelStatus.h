#pragma once
#include <crow.h>
#include <rapidjson/document.h>
namespace insite {
// Get all kernelStatuses from the nest-server
rapidjson::Value NestGetKernelStatuses();

crow::response KernelStatus();
}  // namespace insite
