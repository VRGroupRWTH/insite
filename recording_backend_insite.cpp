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
constexpr double DOUBLE_FILL_VALUE = std::numeric_limits<double>::quiet_NaN();
const H5::DataSpace INITIAL_DATA_SPACE{2, INITIAL_SIZE, MAX_SIZE};
constexpr hsize_t CHUNK_SIZE[] = {100, 100};
}  // namespace

RecordingBackendInsite::RecordingBackendInsite() {
  data_set_properties_.setChunk(2, CHUNK_SIZE);
  data_set_properties_.setFillValue(H5::PredType::NATIVE_DOUBLE,
                                    &DOUBLE_FILL_VALUE);
  data_set_properties_.setFillTime(H5D_FILL_TIME_IFSET);

  http_listener_.support("test", [](web::http::http_request request) {
    web::http::http_response response(200);
    web::json::value response_body("Hello World!");
    response.set_body(response_body);
    request.reply(response);
  });
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
}

void RecordingBackendInsite::disenroll(const nest::RecordingDevice& device) {
  std::cout << "RecordingBackendInsite::disenroll()\n";
}

void RecordingBackendInsite::set_value_names(
    const nest::RecordingDevice& device,
    const std::vector<Name>& double_value_names,
    const std::vector<Name>& long_value_names) {
  std::cout << "RecordingBackendInsite::set_value_names()\n";

  device_attributes_[device.get_name()].double_attribute_names =
      double_value_names;
  device_attributes_[device.get_name()].long_attribute_names = long_value_names;

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
  constexpr hsize_t neuron_ids_initial_size[] = {0};
  constexpr hsize_t neuron_ids_max_size[] = {H5S_UNLIMITED};
  const H5::DataSpace neuron_ids_data_space{1, neuron_ids_initial_size,
                                            neuron_ids_max_size};

  const hsize_t neuron_ids_chunk_size[] = {CHUNK_SIZE[0]};
  H5::DSetCreatPropList neuron_ids_set_properties_;
  neuron_ids_set_properties_.setChunk(1, neuron_ids_chunk_size);

  neuron_ids_data_set_ = h5_file_->createDataSet(
      "neuron_ids", H5::PredType::NATIVE_UINT64, neuron_ids_data_space,
      neuron_ids_set_properties_);
}

void RecordingBackendInsite::cleanup() {
  std::cout << "RecordingBackendInsite::cleanup()\n";
  ++run_count_;
  h5_file_->close();
  h5_file_.reset();
  device_attributes_.clear();
  double_attributes_.clear();
  double_buffer_.clear();
  long_attributes_.clear();
  long_buffer_.clear();
  data_sets_.clear();
  current_timestep_ = 0;
}

void RecordingBackendInsite::pre_run_hook() {
  std::cout << "RecordingBackendInsite::pre_run_hook()\n";
}

void RecordingBackendInsite::post_run_hook() {
  std::cout << "RecordingBackendInsite::post_run_hook()\n";
}

void RecordingBackendInsite::post_step_hook() {
  // std::cout << "RecordingBackendInsite::post_step_hook()\n";

  const hsize_t data_set_extend[] = {neuron_ids_.size(), current_timestep_ + 1};

  const hsize_t count[2] = {neuron_ids_.size(), 1};
  const hsize_t dest_start[2] = {0, current_timestep_};

  const hsize_t src_space_extend[2] = {neuron_ids_.size(), 1};
  const H5::DataSpace src_space{2, src_space_extend};

  if (double_buffer_.size() > 0) {
    assert(double_buffer_.size() ==
           neuron_ids_.size() * double_attributes_.size());
    for (const auto& x : data_sets_) {
      x.second.extend(data_set_extend);

      const auto dest_space = x.second.getSpace();
      dest_space.selectHyperslab(H5S_SELECT_SET, count, dest_start);

      const hsize_t src_start[2] = {GetDoubleAttributeIndex(x.first), 0};
      const hsize_t src_stride[2] = {double_attributes_.size(), 1};
      src_space.selectHyperslab(H5S_SELECT_SET, count, src_start, src_stride);
      x.second.write(double_buffer_.data(), H5::PredType::NATIVE_DOUBLE,
                     src_space, dest_space);
    }

    ++current_timestep_;
  }
}

void RecordingBackendInsite::write(const nest::RecordingDevice& device,
                                   const nest::Event& event,
                                   const std::vector<double>& double_values,
                                   const std::vector<long>& long_values) {
  // std::cout << "RecordingBackendInsite::write()\n";
  const auto neuron_id = event.get_sender_gid();
  const auto neuron_index = GetNeuronIndex(neuron_id);
  if (neuron_index >= neuron_ids_.size()) {
    neuron_ids_.push_back(neuron_id);
    double_buffer_.resize(neuron_ids_.size() * double_attributes_.size(),
                          std::numeric_limits<double>::quiet_NaN());
    long_buffer_.resize(neuron_ids_.size() * long_attributes_.size(),
                        std::numeric_limits<long>::max());

    hsize_t length[] = {neuron_ids_.size()};
    neuron_ids_data_set_.extend(length);
    neuron_ids_data_set_.write(neuron_ids_.data(), H5::PredType::NATIVE_UINT64);
  }

  const auto& device_attributes = device_attributes_.at(device.get_name());

  assert(device_attributes.double_attribute_names.size() ==
         double_values.size());
  for (size_t i = 0; i < double_values.size(); ++i) {
    const auto& attribute_name = device_attributes.double_attribute_names[i];
    const auto attribute_index = GetDoubleAttributeIndex(attribute_name);
    double_buffer_[neuron_index * double_attributes_.size() + attribute_index] =
        double_values[i];
  }

  assert(device_attributes.long_attribute_names.size() == long_values.size());
  for (size_t i = 0; i < long_values.size(); ++i) {
    const auto& attribute_name = device_attributes.long_attribute_names[i];
    const auto attribute_index = GetLongAttributeIndex(attribute_name);
    long_buffer_[neuron_index * long_attributes_.size() + attribute_index] =
        long_values[i];
  }

  // std::cout << event.get_stamp().get_steps() << "\t" << event.get_offset() <<
  // ":" << event.get_sender_gid() << "\n";
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
