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
URL_NEST_GET_SPIKES_BY_SPIKERECORDER_ID = BASE_REQUEST_URL + "/spikerecorders"
URL_NEST_GET_SPIKES_BY_SPIKERECORDER_ID_V2 = BASE_REQUEST_URL_V2 + "/spikerecorders"

#Names for the nest_get_spikes_by_spikerecorder query-parameters
class NEST_GET_SPIKES_BY_SPIKERECORDER_PARAMETER_NAME_LIST (Enum):
    fromTime = "fromTime"
    toTime = "toTime"
    nodeIds = "nodeIds"
    skip = "skip"
    top = "top"

#Sends a nest_get_spikes_by_spikerecorder request to get all included spike-recorders and returns them
def get_nest_spikerecorder_ids():
    spike_recorder_list = return_json_body_if_status_ok(URL_NEST_GET_SPIKES_BY_SPIKERECORDER_ID)
    id_list = []

    for spike_recorder in spike_recorder_list:
        assert(JSON_VALUE_TO_FIELD_NAME.nodeIds.value in spike_recorder)
        assert(JSON_VALUE_TO_FIELD_NAME.spikerecorderId.value in spike_recorder)
        assert(len(spike_recorder) == 2)

        id_list.append(spike_recorder[JSON_VALUE_TO_FIELD_NAME.spikerecorderId.value])
    
    return id_list

def get_nest_spikerecorder_ids_v2():
    json_response = return_json_body_if_status_ok(URL_NEST_GET_SPIKES_BY_SPIKERECORDER_ID_V2)
    assert(JSON_VALUE_TO_FIELD_NAME.simulationId.value in json_response)

    spike_recorder_list = json_response[JSON_VALUE_TO_FIELD_NAME.spikerecorders.value]
    id_list = []

    for spike_recorder in spike_recorder_list:
        assert(JSON_VALUE_TO_FIELD_NAME.nodeIds.value in spike_recorder)
        assert(JSON_VALUE_TO_FIELD_NAME.spikerecorderId.value in spike_recorder)
        assert(len(spike_recorder) == 2)

        id_list.append(spike_recorder[JSON_VALUE_TO_FIELD_NAME.spikerecorderId.value])
    
    return id_list

#Combines the request-base-URL, a spike-recorder ID and the JSON-key for spikes to a String and returns it
def build_url_nest_get_spikes_by_spikerecorder(id):
    return (URL_NEST_GET_SPIKES_BY_SPIKERECORDER_ID + "/" + str(id) + "/" + JSON_VALUE_TO_FIELD_NAME.spikes.value + "/")

def build_url_nest_get_spikes_by_spikerecorder_v2(id):
    return (URL_NEST_GET_SPIKES_BY_SPIKERECORDER_ID_V2 + "/" + str(id) + "/" + JSON_VALUE_TO_FIELD_NAME.spikes.value + "/")

#Sends a nest_get_spikes_by_spikerecorder request by using a spike-recorder ID and values for all the possible parameters and returns the result in JSON-format
def request_nest_get_spikes_by_spikerecorder(id, parameter_values = None, parameter_set_combination = [False, False, False, False, False]):
    prefix = build_url_nest_get_spikes_by_spikerecorder(id)
    return return_json_body_if_status_ok(build_query_string(prefix, NEST_GET_SPIKES_BY_SPIKERECORDER_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))

def request_nest_get_spikes_by_spikerecorder_v2(id, parameter_values = None, parameter_set_combination = [False, False, False, False, False]):
    prefix = build_url_nest_get_spikes_by_spikerecorder_v2(id)
    return return_json_body_if_status_ok(build_query_string(prefix, NEST_GET_SPIKES_BY_SPIKERECORDER_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))

#Tests a default nest_get_spikes_by_spikerecorder request without parameters for every possible spikerecorder
def test_nest_get_spikes_by_spikerecorder_no_parameters():
    spikerecorder_node_ids = get_nest_spikerecorder_ids()

    for id in spikerecorder_node_ids:
        spike_data = request_nest_get_spikes_by_spikerecorder(id)
        spikes_is_valid_format(spike_data)

def test_nest_get_spikes_by_spikerecorder_no_parameters_v2():
    spikerecorder_node_ids = get_nest_spikerecorder_ids_v2()

    for id in spikerecorder_node_ids:
        spike_data = request_nest_get_spikes_by_spikerecorder_v2(id)
        spikes_is_valid_format(spike_data)

#Tests every possible nest_get_spikes_by_spikerecorder request with the "fromTime" parameter by checking if all returned times are greater or equal than requested for every possible spikerecorder
def test_nest_get_spikes_parameter_fromTime(): 
    parameter_values = [15, 0, 0, 0, 0]
    parameter_set_combination = [True, False, False, False, False]
    spikerecorder_ids = get_nest_spikerecorder_ids()

    for id in spikerecorder_ids:
        spike_data = request_nest_get_spikes_by_spikerecorder(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_simulation_times_are_greater_or_equal_than(spike_data, parameter_values[0])

def test_nest_get_spikes_parameter_fromTime_v2(): 
    parameter_values = [15, 0, 0, 0, 0]
    parameter_set_combination = [True, False, False, False, False]
    spikerecorder_ids = get_nest_spikerecorder_ids_v2()

    for id in spikerecorder_ids:
        spike_data = request_nest_get_spikes_by_spikerecorder_v2(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_simulation_times_are_greater_or_equal_than(spike_data, parameter_values[0])

#Tests a nest_get_spikes_by_spikerecorder request with the "toTime" parameter by checking if all returned times are smaller or equal than requested for every possible spikerecorder
def test_nest_get_spikes_parameter_toTime(): 
    parameter_values = [0, 25, 0, 0, 0]
    parameter_set_combination = [False, True, False, False, False]
    spikerecorder_ids = get_nest_spikerecorder_ids()

    for id in spikerecorder_ids:
        spike_data = request_nest_get_spikes_by_spikerecorder(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_simulation_times_are_smaller_or_equal_than(spike_data, parameter_values[1])

def test_nest_get_spikes_parameter_toTime_v2(): 
    parameter_values = [0, 25, 0, 0, 0]
    parameter_set_combination = [False, True, False, False, False]
    spikerecorder_ids = get_nest_spikerecorder_ids_v2()

    for id in spikerecorder_ids:
        spike_data = request_nest_get_spikes_by_spikerecorder_v2(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_simulation_times_are_smaller_or_equal_than(spike_data, parameter_values[1])

#Tests a nest_get_spikes_by_spikerecorder request with the "fromTime" and the "toTime" parameter by checking if all returned times are in the requested timespan for every possible spikerecorder
def test_nest_get_spikes_parameter_fromTime_toTime(): 
    parameter_values = [10, 25, 0, 0, 0]
    parameter_set_combination = [True, True, False, False, False]
    spikerecorder_ids = get_nest_spikerecorder_ids()

    for id in spikerecorder_ids:
        spike_data = request_nest_get_spikes_by_spikerecorder(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_simulation_times_are_greater_or_equal_than(spike_data, parameter_values[0])
        spikes_simulation_times_are_smaller_or_equal_than(spike_data, parameter_values[1]) 

def test_nest_get_spikes_parameter_fromTime_toTime_v2(): 
    parameter_values = [10, 25, 0, 0, 0]
    parameter_set_combination = [True, True, False, False, False]
    spikerecorder_ids = get_nest_spikerecorder_ids_v2()

    for id in spikerecorder_ids:
        spike_data = request_nest_get_spikes_by_spikerecorder_v2(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_simulation_times_are_greater_or_equal_than(spike_data, parameter_values[0])
        spikes_simulation_times_are_smaller_or_equal_than(spike_data, parameter_values[1]) 

#Tests a nest_get_spikes_by_spikerecorder request with the "nodeIds" parameter by checking if only the requested nodeIDs are returned for every possible spikerecorder
def test_nest_get_spikes_parameter_nodeIds():
    node_ids = [11, 101, 2759]
    parameter_set_combination = [False, False, True, False, False]
    spikerecorder_ids = get_nest_spikerecorder_ids()

    for id in spikerecorder_ids:
        data = return_json_body_if_status_ok(build_url_nest_get_spikes_by_spikerecorder(id))
        node_ids.append(data[JSON_VALUE_TO_FIELD_NAME.nodeIds.value][0])

        parameter_values = [0, 0, node_ids, 0, 0]

        spike_data = request_nest_get_spikes_by_spikerecorder(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_nodeIds_are_subset(spike_data, node_ids)

def test_nest_get_spikes_parameter_nodeIds_v2():
    node_ids = [11, 101, 2759]
    parameter_set_combination = [False, False, True, False, False]
    spikerecorder_ids = get_nest_spikerecorder_ids_v2()

    for id in spikerecorder_ids:
        data = return_json_body_if_status_ok(build_url_nest_get_spikes_by_spikerecorder_v2(id))
        node_ids.append(data[JSON_VALUE_TO_FIELD_NAME.nodeIds.value][0])

        parameter_values = [0, 0, node_ids, 0, 0]

        spike_data = request_nest_get_spikes_by_spikerecorder_v2(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_nodeIds_are_subset(spike_data, node_ids)

#Test a nest_get_spikes_by_spikerecorder request with the "skip" parameter by checking if a request without the "skip" parameter returns the same result but offset for every possible spikerecorder
@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_parameter_skip():
    parameter_values = [0, 0, 0, 4, 0]
    parameter_set_combination = [False, False, False, True, False]
    spikerecorder_node_ids = get_nest_spikerecorder_ids()

    for id in spikerecorder_node_ids:
        spike_data = request_nest_get_spikes_by_spikerecorder(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_has_offset_in_comparison_to(build_url_nest_get_spikes_by_spikerecorder(id), spike_data, NEST_GET_SPIKES_BY_SPIKERECORDER_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination)

@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_parameter_skip_v2():
    parameter_values = [0, 0, 0, 4, 0]
    parameter_set_combination = [False, False, False, True, False]
    spikerecorder_node_ids = get_nest_spikerecorder_ids_v2()

    for id in spikerecorder_node_ids:
        spike_data = request_nest_get_spikes_by_spikerecorder_v2(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_has_offset_in_comparison_to(build_url_nest_get_spikes_by_spikerecorder_v2(id), spike_data, NEST_GET_SPIKES_BY_SPIKERECORDER_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination)

#Test a nest_get_spikes_by_spikerecorder request with the "top" parameter by checking if the number of entries is smaller or equal than the desired amount for every possible spikerecorder
def test_nest_get_spikes_parameter_top():
    parameter_values = [0, 0, 0, 0, 23]
    parameter_set_combination = [False, False, False, False, True]
    spikerecorder_node_ids = get_nest_spikerecorder_ids()

    for id in spikerecorder_node_ids:
        spike_data = request_nest_get_spikes_by_spikerecorder(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_length_less_or_equal_to(spike_data, parameter_values[4])

def test_nest_get_spikes_parameter_top_v2():
    parameter_values = [0, 0, 0, 0, 23]
    parameter_set_combination = [False, False, False, False, True]
    spikerecorder_node_ids = get_nest_spikerecorder_ids_v2()

    for id in spikerecorder_node_ids:
        spike_data = request_nest_get_spikes_by_spikerecorder_v2(id, parameter_values, parameter_set_combination)
        spikes_is_valid_format(spike_data)
        spikes_length_less_or_equal_to(spike_data, parameter_values[4])

#Tests a nest_get_spikes_by_spikerecorder request with the paramaters: "fromTime", "toTime", "nodeIds", "skip" and "top" by checking if the conditions for each of the parameters apply to the returned data for every possible spikerecorder
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
    spikerecorder_ids = get_nest_spikerecorder_ids()

    for id in spikerecorder_ids:
        data = return_json_body_if_status_ok(build_url_nest_get_spikes_by_spikerecorder(id))
        node_id = data[JSON_VALUE_TO_FIELD_NAME.nodeIds.value][0]
        paramater_values[2].append(node_id)

        spike_data = request_nest_get_spikes_by_spikerecorder(id, paramater_values, [True, True, True, True, True])
        spikes_is_valid_format(spike_data)
        
        spikes_simulation_times_are_greater_or_equal_than(spike_data, paramater_values[0])
        spikes_simulation_times_are_smaller_or_equal_than(spike_data, paramater_values[1])
        spikes_nodeIds_are_subset(spike_data, paramater_values[2])
        spikes_has_offset_in_comparison_to(build_url_nest_get_spikes_by_spikerecorder(id), spike_data, NEST_GET_SPIKES_BY_SPIKERECORDER_PARAMETER_NAME_LIST, paramater_values)
        spikes_length_less_or_equal_to(spike_data, paramater_values[4])

@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_all_parameters_v2():
    paramater_values = [
        2,
        40,
        [3084, 2941, 2502, 2607, 2807],
        0,
        2000
    ]
    paramater_values = list(paramater_values)
    spikerecorder_ids = get_nest_spikerecorder_ids_v2()

    for id in spikerecorder_ids:
        data = return_json_body_if_status_ok(build_url_nest_get_spikes_by_spikerecorder_v2(id))
        node_id = data[JSON_VALUE_TO_FIELD_NAME.nodeIds.value][0]
        paramater_values[2].append(node_id)

        spike_data = request_nest_get_spikes_by_spikerecorder_v2(id, paramater_values, [True, True, True, True, True])
        spikes_is_valid_format(spike_data)
        
        spikes_simulation_times_are_greater_or_equal_than(spike_data, paramater_values[0])
        spikes_simulation_times_are_smaller_or_equal_than(spike_data, paramater_values[1])
        spikes_nodeIds_are_subset(spike_data, paramater_values[2])
        spikes_has_offset_in_comparison_to(build_url_nest_get_spikes_by_spikerecorder(id), spike_data, NEST_GET_SPIKES_BY_SPIKERECORDER_PARAMETER_NAME_LIST, paramater_values)
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

    spikerecorder_ids = get_nest_spikerecorder_ids()

    for id in spikerecorder_ids:
        data = return_json_body_if_status_ok(build_url_nest_get_spikes_by_spikerecorder(id))
        node_id = data[JSON_VALUE_TO_FIELD_NAME.nodeIds.value][0]
        parameter_values[2].append(node_id)

        query_url = build_url_nest_get_spikes_by_spikerecorder(id)
        check_all_parameter_combinations(query_url, NEST_GET_SPIKES_BY_SPIKERECORDER_PARAMETER_NAME_LIST, paramater_values)

@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_parameter_combinations_v2():
    paramater_values = [
        2,
        40,
        [3084, 2941, 2502, 2607, 2807],
        0,
        2000
    ]
    parameter_values = list(paramater_values)

    spikerecorder_ids = get_nest_spikerecorder_ids_v2()

    for id in spikerecorder_ids:
        data = return_json_body_if_status_ok(build_url_nest_get_spikes_by_spikerecorder_v2(id))
        node_id = data[JSON_VALUE_TO_FIELD_NAME.nodeIds.value][0]
        parameter_values[2].append(node_id)

        query_url = build_url_nest_get_spikes_by_spikerecorder_v2(id)
        check_all_parameter_combinations(query_url, NEST_GET_SPIKES_BY_SPIKERECORDER_PARAMETER_NAME_LIST, paramater_values)

@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_last_frame_sim_finished():
    end_time = return_json_body_if_status_ok(BASE_REQUEST_URL + '/simulationTimeInfo')['end']
    
    spikerecorder_ids = get_nest_spikerecorder_ids()
    spikerecorder_id = spikerecorder_ids[0]

    spikes_without_last = return_json_body_if_status_ok(build_query_string(build_url_nest_get_spikes_by_spikerecorder(spikerecorder_id),[NEST_GET_SPIKES_BY_SPIKERECORDER_PARAMETER_NAME_LIST.toTime],[end_time - 1.0]))
    assert(spikes_without_last['lastFrame'] == 0)

    spikes_without_last = return_json_body_if_status_ok(build_query_string(build_url_nest_get_spikes_by_spikerecorder(spikerecorder_id)))
    assert(spikes_without_last['lastFrame'] == 1)

@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_last_frame_sim_finished_v2():
    end_time = return_json_body_if_status_ok(BASE_REQUEST_URL + '/simulationTimeInfo')['end']
    
    spikerecorder_ids = get_nest_spikerecorder_ids_v2()
    spikerecorder_id = spikerecorder_ids[0]

    spikes_without_last = return_json_body_if_status_ok(build_query_string(build_url_nest_get_spikes_by_spikerecorder_v2(spikerecorder_id),[NEST_GET_SPIKES_BY_SPIKERECORDER_PARAMETER_NAME_LIST.toTime],[end_time - 1.0]))
    assert(spikes_without_last['lastFrame'] == 0)

    spikes_without_last = return_json_body_if_status_ok(build_query_string(build_url_nest_get_spikes_by_spikerecorder_v2(spikerecorder_id)))
    assert(spikes_without_last['lastFrame'] == 1)

@pytest.mark.order("first")
def test_nest_get_spikes_last_frame_while_running(nest_simulation, printer):
    simulation_time_info = return_json_body_if_status_ok(BASE_REQUEST_URL + '/simulationTimeInfo')
    end_time = simulation_time_info['end']
    curr_time = simulation_time_info['current']
    if curr_time == end_time:
        return
    
    spikerecorder_ids = get_nest_spikerecorder_ids()
    spikerecorder_id = spikerecorder_ids[0] 

    spikes_without_last = return_json_body_if_status_ok(build_query_string(build_url_nest_get_spikes_by_spikerecorder(spikerecorder_id)))
    assert(spikes_without_last['lastFrame'] == False)

@pytest.mark.order("first")
def test_nest_get_spikes_last_frame_while_running_v2(nest_simulation, printer):
    simulation_time_info = return_json_body_if_status_ok(BASE_REQUEST_URL + '/simulationTimeInfo')
    end_time = simulation_time_info['end']
    curr_time = simulation_time_info['current']
    if curr_time == end_time:
        return
    
    spikerecorder_ids = get_nest_spikerecorder_ids_v2()
    spikerecorder_id = spikerecorder_ids[0] 

    spikes_without_last = return_json_body_if_status_ok(build_query_string(build_url_nest_get_spikes_by_spikerecorder_v2(spikerecorder_id)))
    assert(spikes_without_last['lastFrame'] == False)
