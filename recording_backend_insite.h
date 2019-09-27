/*
 *  recording_backend_socket.h
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

#ifndef RECORDING_BACKEND_INSITE_H
#define RECORDING_BACKEND_INSITE_H

#include <H5Cpp.h>
#include <cstdint>
#include <memory>
#include <vector>

#include "recording_backend.h"

namespace insite {

class RecordingBackendInsite : public nest::RecordingBackend {
 public:
  RecordingBackendInsite();

  ~RecordingBackendInsite() throw() override;

  void initialize() override;
  void finalize() override;

  void enroll(const nest::RecordingDevice& device,
              const DictionaryDatum& params) override;

  void disenroll(const nest::RecordingDevice& device) override;

  void set_value_names(const nest::RecordingDevice& device,
                       const std::vector<Name>& double_value_names,
                       const std::vector<Name>& long_value_names) override;

  void prepare() override;

  void cleanup() override;

  void pre_run_hook() override;

  void post_run_hook() override;

  void post_step_hook() override;

  void write(const nest::RecordingDevice& device, const nest::Event& event,
             const std::vector<double>& double_values,
             const std::vector<long>& long_values) override;

  void set_status(const DictionaryDatum& params) override;

  void get_status(DictionaryDatum& params) const override;

  void check_device_status(const DictionaryDatum& params) const override;

  void get_device_defaults(DictionaryDatum& params) const override;

  void get_device_status(const nest::RecordingDevice& device,
                         DictionaryDatum& params) const override;

 private:
  std::vector<std::uint64_t> neuron_ids_;
  std::vector<Name> double_attributes_;
  std::vector<double> double_buffer_;
  std::vector<Name> long_attributes_;
  std::vector<long> long_buffer_;
  unsigned int current_timestep_ = 0;
  unsigned int run_count_ = 0;

  struct AttributeNames {
    std::vector<Name> double_attribute_names;
    std::vector<Name> long_attribute_names;
  };
  std::map<std::string, AttributeNames> device_attributes_;

  H5::DSetCreatPropList data_set_properties_;
  std::unique_ptr<H5::H5File> h5_file_;
  std::map<std::string, H5::DataSet> data_sets_;
  H5::DataSet neuron_ids_data_set_;

  inline std::size_t GetNeuronIndex(std::uint64_t neuron_id) const {
    return std::find(neuron_ids_.begin(), neuron_ids_.end(), neuron_id) -
           neuron_ids_.begin();
  }

  inline std::size_t GetDoubleAttributeIndex(const Name& attribute) {
    return std::find(double_attributes_.begin(), double_attributes_.end(),
                     attribute) -
           double_attributes_.begin();
  }

  inline std::size_t GetLongAttributeIndex(const Name& attribute) {
    return std::find(long_attributes_.begin(), long_attributes_.end(),
                     attribute) -
           long_attributes_.begin();
  }
};

}  // namespace insite

#endif  // RECORDING_BACKEND_INSITE_H
