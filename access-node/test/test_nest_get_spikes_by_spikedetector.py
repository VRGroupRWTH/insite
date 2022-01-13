import time
import numbers
import math
import itertools
from operator import truediv
import requests
from enum import Enum
from requests.api import request
from requests.sessions import Request
from nest_general_test_functions import *
import pytest

#URL used for the "nest_get_spikes" HTTP-query
URL_NEST_GET_SPIKES_BY_SPIKEDETECTOR_ID = BASE_REQUEST_URL + "/spikedetectors"

#Names for the nest_get_spikes_by_spikedetector query-parameters
class NEST_GET_SPIKES_BY_SPIKEDETECTOR_PARAMETER_NAME_LIST (Enum):
    fromTime = "fromTime"
    toTime = "toTime"
    nodeIds = "nodeIds"
    skip = "skip"
    top = "top"

#Sends a nest_get_spikes_by_spikedetector request to get all included spike-detectors and returns them
def get_nest_spikedetector_ids():
    spike_detector_list = return_json_body_if_status_ok(URL_NEST_GET_SPIKES_BY_SPIKEDETECTOR_ID)
    id_list = []

    for spike_detector in spike_detector_list:
        assert(JSON_VALUE_TO_FIELD_NAME.nodeIds.value in spike_detector)
        assert(JSON_VALUE_TO_FIELD_NAME.spikedetectorId.value in spike_detector)
        assert(len(spike_detector) == 2)

        id_list.append(spike_detector[JSON_VALUE_TO_FIELD_NAME.spikedetectorId.value])
    
    return id_list

#Combines the request-base-URL, a spike-detector ID and the JSON-key for spikes to a String and returns it
def build_url_nest_get_spikes_by_spikedetector(id):
    return (URL_NEST_GET_SPIKES_BY_SPIKEDETECTOR_ID + "/" + str(id) + "/" + JSON_VALUE_TO_FIELD_NAME.spikes.value)

#Sends a nest_get_spikes_by_spikedetector request by using a spike-detector ID and values for all the possible parameters and returns the result in JSON-format
def request_nest_get_spikes_by_spikedetector(id, parameter_values, parameter_set_combination = [False, False, False, False, False]):
    prefix = build_url_nest_get_spikes_by_spikedetector(id)
    return return_json_body_if_status_ok(build_query_string(prefix, NEST_GET_SPIKES_BY_SPIKEDETECTOR_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))

#Tests a default nest_get_spikes_by_spikedetector request without parameters for every possible spikedetector
def test_nest_get_spikes_by_spikedetector_no_parameters():
    spikedetector_node_ids = get_nest_spikedetector_ids()

    for id in spikedetector_node_ids:
        spike_data = request_nest_get_spikes_by_spikedetector(id, [])
        spikes_is_valid_format(spike_data)

#Tests every possible nest_get_spikes_by_spikedetector request with the "fromTime" parameter by checking if all returned times are greater or equal than requested for every possible spikedetector
def test_nest_get_spikes_parameter_fromTime(): 
    parameter_values = [15, 0, 0, 0, 0]
    parameter_set_combination = [True, False, False, False, False]
    spikedetector_ids = get_nest_spikedetector_ids()

    for id in spikedetector_ids:
        spike_data = request_nest_get_spikes_by_spikedetector(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_simulation_times_are_greater_or_equal_than(spike_data, parameter_values[0])

#Tests a nest_get_spikes_by_spikedetector request with the "toTime" parameter by checking if all returned times are smaller or equal than requested for every possible spikedetector
def test_nest_get_spikes_parameter_toTime(): 
    parameter_values = [0, 25, 0, 0, 0]
    parameter_set_combination = [False, True, False, False, False]
    spikedetector_ids = get_nest_spikedetector_ids()

    for id in spikedetector_ids:
        spike_data = request_nest_get_spikes_by_spikedetector(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_simulation_times_are_smaller_or_equal_than(spike_data, parameter_values[1])

#Tests a nest_get_spikes_by_spikedetector request with the "fromTime" and the "toTime" parameter by checking if all returned times are in the requested timespan for every possible spikedetector
def test_nest_get_spikes_parameter_fromTime_toTime(): 
    parameter_values = [10, 25, 0, 0, 0]
    parameter_set_combination = [True, True, False, False, False]
    spikedetector_ids = get_nest_spikedetector_ids()

    for id in spikedetector_ids:
        spike_data = request_nest_get_spikes_by_spikedetector(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_simulation_times_are_greater_or_equal_than(spike_data, parameter_values[0])
        spikes_simulation_times_are_smaller_or_equal_than(spike_data, parameter_values[1]) 

#Tests a nest_get_spikes_by_spikedetector request with the "nodeIds" parameter by checking if only the requested nodeIDs are returned for every possible spikedetector
def test_nest_get_spikes_parameter_nodeIds():
    node_ids = [11, 101, 2759]
    parameter_set_combination = [False, False, True, False, False]
    spikedetector_ids = get_nest_spikedetector_ids()

    for id in spikedetector_ids:
        data = return_json_body_if_status_ok(build_url_nest_get_spikes_by_spikedetector(id))
        node_ids.append(data[JSON_VALUE_TO_FIELD_NAME.nodeIds.value][0])

        parameter_values = [0, 0, node_ids, 0, 0]

        spike_data = request_nest_get_spikes_by_spikedetector(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_nodeIds_are_subset(spike_data, node_ids)

#Test a nest_get_spikes_by_spikedetector request with the "skip" parameter by checking if a request without the "skip" parameter returns the same result but offset for every possible spikedetector
@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_parameter_skip():
    parameter_values = [0, 0, 0, 4, 0]
    parameter_set_combination = [False, False, False, True, False]
    spikedetector_node_ids = get_nest_spikedetector_ids()

    for id in spikedetector_node_ids:
        spike_data = request_nest_get_spikes_by_spikedetector(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_has_offset_in_comparison_to(build_url_nest_get_spikes_by_spikedetector(id), spike_data, NEST_GET_SPIKES_BY_SPIKEDETECTOR_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination)

#Test a nest_get_spikes_by_spikedetector request with the "top" parameter by checking if the number of entries is smaller or equal than the desired amount for every possible spikedetector
def test_nest_get_spikes_parameter_top():
    parameter_values = [0, 0, 0, 0, 23]
    parameter_set_combination = [False, False, False, False, True]
    spikedetector_node_ids = get_nest_spikedetector_ids()

    for id in spikedetector_node_ids:
        spike_data = request_nest_get_spikes_by_spikedetector(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_length_less_or_equal_to(spike_data, parameter_values[4])

#Tests a nest_get_spikes_by_spikedetector request with the paramaters: "fromTime", "toTime", "nodeIds", "skip" and "top" by checking if the conditions for each of the parameters apply to the returned data for every possible spikedetector
@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_all_parameters():
    paramater_values = [
        2,
        40,
        [3084, 2941, 2502, 2607, 2807],
        0,
        2000
    ]
    paramater_values = list(paramater_values)
    spikedetector_ids = get_nest_spikedetector_ids()

    for id in spikedetector_ids:
        data = return_json_body_if_status_ok(build_url_nest_get_spikes_by_spikedetector(id))
        node_id = data[JSON_VALUE_TO_FIELD_NAME.nodeIds.value][0]
        paramater_values[2].append(node_id)

        spike_data = request_nest_get_spikes_by_spikedetector(id, paramater_values, [True, True, True, True, True])
        spikes_is_valid_format(spike_data)
        
        spikes_simulation_times_are_greater_or_equal_than(spike_data, paramater_values[0])
        spikes_simulation_times_are_smaller_or_equal_than(spike_data, paramater_values[1])
        spikes_nodeIds_are_subset(spike_data, paramater_values[2])
        spikes_has_offset_in_comparison_to(build_url_nest_get_spikes_by_spikedetector(id), spike_data, NEST_GET_SPIKES_BY_SPIKEDETECTOR_PARAMETER_NAME_LIST, paramater_values)
        spikes_length_less_or_equal_to(spike_data, paramater_values[4])

#Tests every possible combination of the nest_get_spikes query-parameters "fromTime", "toTime", "nodeIds", "skip" and "top" by checking if the conditions for each of the parameters apply to the returned data
@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_parameter_combinations():
    paramater_values = [
        2,
        40,
        [3084, 2941, 2502, 2607, 2807],
        0,
        2000
    ]
    parameter_values = list(paramater_values)

    spikedetector_ids = get_nest_spikedetector_ids()

    for id in spikedetector_ids:
        data = return_json_body_if_status_ok(build_url_nest_get_spikes_by_spikedetector(id))
        node_id = data[JSON_VALUE_TO_FIELD_NAME.nodeIds.value][0]
        parameter_values[2].append(node_id)

        query_url = build_url_nest_get_spikes_by_spikedetector(id)
        check_all_parameter_combinations(query_url, NEST_GET_SPIKES_BY_SPIKEDETECTOR_PARAMETER_NAME_LIST, paramater_values)


@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_last_frame_sim_finished():
    end_time = return_json_body_if_status_ok(BASE_REQUEST_URL + '/simulationTimeInfo')['end']
    
    spikedetector_ids = get_nest_spikedetector_ids()
    spikedetector_id = spikedetector_ids[0]

    spikes_without_last = return_json_body_if_status_ok(build_query_string(build_url_nest_get_spikes_by_spikedetector(spikedetector_id),[NEST_GET_SPIKES_BY_SPIKEDETECTOR_PARAMETER_NAME_LIST.toTime],[end_time - 1.0]))
    assert(spikes_without_last['lastFrame'] == 0)

    spikes_without_last = return_json_body_if_status_ok(build_query_string(build_url_nest_get_spikes_by_spikedetector(spikedetector_id)))
    assert(spikes_without_last['lastFrame'] == 1)

@pytest.mark.order("first")
def test_nest_get_spikes_last_frame_while_running(nest_simulation,printer):
    simulation_time_info = return_json_body_if_status_ok(BASE_REQUEST_URL + '/simulationTimeInfo')
    end_time = simulation_time_info['end']
    curr_time = simulation_time_info['current']
    if curr_time == end_time:
        return
    
    spikedetector_ids = get_nest_spikedetector_ids()
    spikedetector_id = spikedetector_ids[0] 

    spikes_without_last = return_json_body_if_status_ok(build_query_string(build_url_nest_get_spikes_by_spikedetector(spikedetector_id)))
    assert(spikes_without_last['lastFrame'] == False)
