/*
 *  recording_backend_socket.cpp
 *
 *  This file is part of NEST.
 *
 *  Copyright (C) 2004 The NEST Initiative
 *
 *  NEST is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEST is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEST.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Includes from libnestutil:
#include "compose.hpp"

// Includes from nestkernel:
#include "recording_device.h"
#include "vp_manager_impl.h"

// Includes from sli:
#include "dictutils.h"

#include "recording_backend_insite.h"

namespace insite {

// We are only using C++11 :(
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

namespace {
constexpr hsize_t INITIAL_SIZE[] = {0, 0};
constexpr hsize_t MAX_SIZE[] = {H5S_UNLIMITED, H5S_UNLIMITED};
const H5::DataSpace INITIAL_DATA_SPACE{2, INITIAL_SIZE, MAX_SIZE};
}  // namespace

RecordingBackendInsite::RecordingBackendInsite() {
  const hsize_t CHUNK_SIZE[] = {100, 100};
  data_set_properties_.setChunk(2, CHUNK_SIZE);
}

RecordingBackendInsite::~RecordingBackendInsite() throw() {}

void RecordingBackendInsite::initialize() {
  std::cout << "RecordingBackendInsite::initialize()\n";
}

void RecordingBackendInsite::finalize() {
  std::cout << "RecordingBackendInsite::finalize()\n";
}

void RecordingBackendInsite::enroll(const nest::RecordingDevice& device,
                                    const DictionaryDatum& params) {
  std::cout << "RecordingBackendInsite::enroll()\n";
  if (device.get_type() == nest::RecordingDevice::Type::SPIKE_DETECTOR) {
  }
}

void RecordingBackendInsite::disenroll(const nest::RecordingDevice& device) {
  std::cout << "RecordingBackendInsite::disenroll()\n";
}

void RecordingBackendInsite::set_value_names(
    const nest::RecordingDevice& device,
    const std::vector<Name>& double_value_names,
    const std::vector<Name>& long_value_names) {
  std::cout << "RecordingBackendInsite::set_value_names()\n";

  if (device.get_type() != nest::RecordingDevice::Type::SPIKE_DETECTOR) {
    std::cout << "Device " << device.get_name() << ":\n double valuenames :\n ";
    for (const auto& name : double_value_names) {
      std::cout << "  " << name << "\n";
      if (std::find(double_attributes_.begin(), double_attributes_.end(),
                    name) == double_attributes_.end()) {
        double_attributes_.push_back(name);
      }
    }

    std::cout << " long value names:\n";
    for (const auto& name : long_value_names) {
      std::cout << "  " << name << "\n";
      if (std::find(long_attributes_.begin(), long_attributes_.end(), name) ==
          long_attributes_.end()) {
        long_attributes_.push_back(name);
      }
    }
  }
}

void RecordingBackendInsite::prepare() {
  std::cout << "RecordingBackendInsite::prepare()\n";

  std::string filename = "run-" + std::to_string(run_count_) + ".h5";
  h5_file_ = make_unique<H5::H5File>(filename.c_str(), H5F_ACC_TRUNC);

  for (const auto& name : double_attributes_) {
    const auto& name_string = name.toString();
    data_sets_.insert(std::make_pair(
        name_string, h5_file_->createDataSet(
                         name_string.c_str(), H5::PredType::NATIVE_DOUBLE,
                         INITIAL_DATA_SPACE, data_set_properties_)));
  }

  for (const auto& name : long_attributes_) {
    const auto& name_string = name.toString();
    data_sets_.insert(std::make_pair(
        name_string,
        h5_file_->createDataSet(name_string.c_str(), H5::PredType::NATIVE_LONG,
                                INITIAL_DATA_SPACE, data_set_properties_)));
  }
}

void RecordingBackendInsite::cleanup() {
  std::cout << "RecordingBackendInsite::cleanup()\n";
  ++run_count_;
  h5_file_->close();
  h5_file_.reset();
  double_attributes_.clear();
  long_attributes_.clear();
  data_sets_.clear();
}

void RecordingBackendInsite::pre_run_hook() {
  std::cout << "RecordingBackendInsite::pre_run_hook()\n";
}

void RecordingBackendInsite::post_run_hook() {
  std::cout << "RecordingBackendInsite::post_run_hook()\n";
}

void RecordingBackendInsite::post_step_hook() {
  // std::cout << "RecordingBackendInsite::post_step_hook()\n";
}

void RecordingBackendInsite::write(const nest::RecordingDevice& device,
                                   const nest::Event& event,
                                   const std::vector<double>& double_values,
                                   const std::vector<long>& long_values) {
  // std::cout << "RecordingBackendInsite::write()\n";
}

void RecordingBackendInsite::set_status(const DictionaryDatum& params) {
  std::cout << "RecordingBackendInsite::set_status()\n";
}

void RecordingBackendInsite::get_status(DictionaryDatum& params) const {
  std::cout << "RecordingBackendInsite::get_status()\n";
}

void RecordingBackendInsite::check_device_status(
    const DictionaryDatum& params) const {
  std::cout << "RecordingBackendInsite::check_device_status()\n";
}

void RecordingBackendInsite::get_device_defaults(
    DictionaryDatum& params) const {
  std::cout << "RecordingBackendInsite::get_device_defaults()\n";
}

void RecordingBackendInsite::get_device_status(
    const nest::RecordingDevice& device, DictionaryDatum& params) const {
  std::cout << "RecordingBackendInsite::get_device_status()\n";
}

}  // namespace insite
