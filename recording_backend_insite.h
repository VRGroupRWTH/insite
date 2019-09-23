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

#ifndef RECORDING_BACKEND_SOCKET_H
#define RECORDING_BACKEND_SOCKET_H

// Includes for network access:
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstdint>
#include <vector>

#include "recording_backend.h"

namespace insite {

class RecordingBackendInsite : public nest::RecordingBackend {
 public:
  RecordingBackendInsite();

  ~RecordingBackendInsite() throw();

  virtual void initialize();
  virtual void finalize();

  virtual void enroll(const nest::RecordingDevice& device,
                      const DictionaryDatum& params);

  virtual void disenroll(const nest::RecordingDevice& device);

  virtual void set_value_names(const nest::RecordingDevice& device,
                               const std::vector<Name>& double_value_names,
                               const std::vector<Name>& long_value_names);

  virtual void prepare();

  virtual void cleanup();

  virtual void pre_run_hook();

  virtual void post_run_hook();

  virtual void post_step_hook();

  virtual void write(const nest::RecordingDevice& device,
                     const nest::Event& event,
                     const std::vector<double>& double_values,
                     const std::vector<long>& long_values);

  virtual void set_status(const DictionaryDatum& params);

  virtual void get_status(DictionaryDatum& params) const;

  virtual void check_device_status(const DictionaryDatum& params) const;

  virtual void get_device_defaults(DictionaryDatum& params) const;

  virtual void get_device_status(const nest::RecordingDevice& device,
                                 DictionaryDatum& params) const;

 private:
  std::vector<std::uint64_t> neuron_ids_;
  std::vector<Name> double_attributes_;
  std::vector<double> double_buffer_;
  std::vector<Name> long_attributes_;
  std::vector<long> long_buffer_;
  bool first_run_ = true;
};

}  // namespace insite

#endif  // RECORDING_BACKEND_SOCKET_H
