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

RecordingBackendInsite::RecordingBackendInsite() {}

RecordingBackendInsite::~RecordingBackendInsite() throw() {}
void RecordingBackendInsite::initialize() {}
void RecordingBackendInsite::finalize() {}

void RecordingBackendInsite::enroll(const nest::RecordingDevice& device,
                                    const DictionaryDatum& params) {}

void RecordingBackendInsite::disenroll(const nest::RecordingDevice& device) {}

void RecordingBackendInsite::set_value_names(
    const nest::RecordingDevice& device,
    const std::vector<Name>& double_value_names,
    const std::vector<Name>& long_value_names) {}

void RecordingBackendInsite::prepare() {}

void RecordingBackendInsite::cleanup() {}

void RecordingBackendInsite::pre_run_hook() {}

void RecordingBackendInsite::post_run_hook() {}

void RecordingBackendInsite::post_step_hook() {}

void RecordingBackendInsite::write(const nest::RecordingDevice& device,
                                   const nest::Event& event,
                                   const std::vector<double>& double_values,
                                   const std::vector<long>& long_values) {}

void RecordingBackendInsite::set_status(const DictionaryDatum& params) {}

void RecordingBackendInsite::get_status(DictionaryDatum& params) const {}

void RecordingBackendInsite::check_device_status(
    const DictionaryDatum& params) const {}

void RecordingBackendInsite::get_device_defaults(DictionaryDatum& params) const {}

void RecordingBackendInsite::get_device_status(
    const nest::RecordingDevice& device, DictionaryDatum& params) const {}

}  // namespace insite
