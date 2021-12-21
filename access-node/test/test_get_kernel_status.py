import time
import numbers
import math
import itertools
from operator import truediv
import requests
from requests.api import request
from requests.sessions import Request
from nest_general_test_functions import *
from six import string_types

#URL used for the "nest_get_kernel_status" HTTP-query
URL_NEST_GET_KERNEL_STATUS = BASE_REQUEST_URL + "/kernelStatus"

#Dictionary that matches the names of the Kernel_status_infos to their corresponding data-types
KERNEL_STATUS_JSON_LIST_ENTRIES = {
    "T_max" : float,
    "T_min" : float,
    'adaptive_spike_buffers' : bool,
    'adaptive_target_buffers' : bool,
    "biological_time" : numbers.Number,
    "buffer_size_spike_data" : numbers.Number,
    "buffer_size_target_data" : numbers.Number,
    "data_path" : string_types,
    "data_prefix" : string_types,
    "dict_miss_is_error" : bool,
    "growth_factor_buffer_spike_data" : numbers.Number,
    "growth_factor_buffer_target_data" : numbers.Number,
    "keep_source_table" : bool,
    "local_num_threads" : int,
    "local_spike_counter" : int,
    "max_buffer_size_spike_data" : numbers.Number,
    "max_buffer_size_target_data" : numbers.Number,
    "max_delay" : numbers.Number,
    "max_num_syn_models" : int,
    "max_update_time" : numbers.Number,
    "min_delay" : numbers.Number,
    "min_update_time" : numbers.Number,
    "ms_per_tic" : numbers.Number,
    "network_size" : int,
    "num_connections" : int,
    "num_processes" : int,
    "off_grid_spiking" : bool,
    "overwrite_files" : bool,
    "print_time" : bool,
    "recording_backends" : dict,
    "recv_buffer_size_secondary_events" : numbers.Number,
    "resolution" : numbers.Number,
    "rng_seed" : numbers.Number,
    "rng_type" : string_types,
    "rng_types" : list,
    "send_buffer_size_secondary_events" : numbers.Number,
    "sort_connections_by_source" : bool,
    "structural_plasticity_synapses" : dict,
    "structural_plasticity_update_interval" : numbers.Number,
    "tics_per_ms" : int,
    "tics_per_step" : int,
    "time_communicate_prepare" : numbers.Number,
    "time_construction_connect" : numbers.Number,
    "time_construction_create" : numbers.Number,
    "time_simulate" : numbers.Number,
    "to_do" : int,
    "total_num_virtual_procs" : int,
    "update_time_limit" : numbers.Number,
    "use_compressed_spikes" : bool,
    "use_wfr" : bool,
    "wfr_comm_interval" : numbers.Number,
    "wfr_interpolation_order" : int,
    "wfr_max_iterations" : int,
    "wfr_tol" : numbers.Number
}

#Tests a nest_get_kernel_status by checking if the response code is valid, all required information is included and the has the correct data-type for every kernel_status
def test_get_kernel_status(nest_simulation):

    kernel_statuses = check_request_valid(URL_NEST_GET_KERNEL_STATUS)
    assert (isinstance(kernel_statuses, list))

    for kernel_status in kernel_statuses:
        assert (isinstance(kernel_status, dict))

        for status_info, status_info_type in KERNEL_STATUS_JSON_LIST_ENTRIES.items():
            assert(status_info in kernel_status)
            assert(isinstance(kernel_status[status_info], status_info_type))

        assert (kernel_status['local_num_threads'] == 1)
