#pragma once
#include <cpr/cpr.h>
#include <cstdint>
#include <iterator>
#include <nest/nestMultimeterContainer.h>
#include <optional>
// #include <params.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <unordered_set>
#include <utilityFunctions.h>
#include <vector>
namespace insite {

// Receives a rapidjson-Document and checks if it has all the necessary
// properties for multimeter measurements
void CheckMultimeterMeasurementValid(
    const rapidjson::GenericObject<false, rapidjson::Value> &multimeter);

// Receives a rapidjson-Object and checks if it has all the necessary properties
// for multimeterData
void CheckMultimeterDataValid(
    const rapidjson::GenericObject<false, rapidjson::Value> &multimeter);

// Get all nest multimeters from the nest-server
crow::response MultimeterResponse(int api_version);

// Get all nest multimeters with the given id from the nest-server
MultimeterContainer NestGetMultimeterById(uint64_t requested_multimeter_id);
crow::response MultimeterByIdResponse(int api_version,
                                      uint64_t requested_multimeter_id);

// Get all nest multimeter attributes with the given attribute name that belong
// to the given multimeterId and apply query parameters

// MultimeterValueContainer NestGetMultimeterAttributes(
//     uint64_t multimeter_id,
//     const std::string& attribute_name,
//     MultimeterParameters parameters);

crow::response MultimeterAttributesResponse(const crow::request &req,
                                            int api_version, int multimeter_id,
                                            const std::string &attr_name);

} // namespace insite
