from nest_general_test_functions import *
import pytest

#URL used for the "nest_get_spikes" HTTP-query
URL_NEST_GET_SPIKES = BASE_REQUEST_URL + "/spikes/"
URL_NEST_GET_SPIKES_V2 = BASE_REQUEST_URL_V2 + "/spikes/"

#Names for the "nest_get_spikes" request-parameters
class NEST_GET_SPIKES_PARAMETER_NAME_LIST (Enum):
    fromTime = "fromTime"
    toTime = "toTime"
    nodeIds = "nodeIds"
    skip = "skip"
    top = "top"

#Tests a default nest_get_spikes request without any parameters
def test_nest_get_spikes_no_parameters():
    spikes_is_valid_format(return_json_body_if_status_ok(URL_NEST_GET_SPIKES))

#Tests a nest_get_spikes request with the "fromTime" parameter by checking if all returned times are greater or equal than requested
def test_nest_get_spikes_parameter_fromTime(): 
    parameter_values = [15, 0, 0, 0, 0]
    parameter_set_combination = [True, False, False, False, False]

    filtered_spikes = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_is_valid_format(filtered_spikes)

    spikes_simulation_times_are_greater_or_equal_than(filtered_spikes, parameter_values[0])

def test_nest_get_spikes_parameter_fromTime_v2(): 
    parameter_values = [15, 0, 0, 0, 0]
    parameter_set_combination = [True, False, False, False, False]

    filtered_spikes = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES_V2, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_is_valid_format(filtered_spikes)

    spikes_simulation_times_are_greater_or_equal_than(filtered_spikes, parameter_values[0])
    assert(filtered_spikes[JSON_VALUE_TO_FIELD_NAME.simulationId.value] == "0:1")

#Tests a nest_get_spikes request with the "toTime" parameter by checking if all returned times are smaller or equal than requested
def test_nest_get_spikes_parameter_toTime(): 
    parameter_values = [0, 25, 0, 0, 0]
    parameter_set_combination = [False, True, False, False, False]

    filtered_spikes = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_is_valid_format(filtered_spikes)

    spikes_simulation_times_are_smaller_or_equal_than(filtered_spikes, parameter_values[1])

def test_nest_get_spikes_parameter_toTime_v2(): 
    parameter_values = [0, 25, 0, 0, 0]
    parameter_set_combination = [False, True, False, False, False]

    filtered_spikes = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES_V2, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_is_valid_format(filtered_spikes)

    spikes_simulation_times_are_smaller_or_equal_than(filtered_spikes, parameter_values[1])
    assert(filtered_spikes[JSON_VALUE_TO_FIELD_NAME.simulationId.value] == "0:1")


#Tests a nest_get_spikes request with the "fromTime" and the "toTime" parameter by checking if all returned times are in thr requested timespan
def test_nest_get_spikes_parameter_fromTime_toTime(): 
    parameter_values = [10, 25, 0, 0, 0]
    parameter_set_combination = [True, True, False, False, False]

    filtered_spikes = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_is_valid_format(filtered_spikes)

    spikes_simulation_times_are_greater_or_equal_than(filtered_spikes, parameter_values[0])
    spikes_simulation_times_are_smaller_or_equal_than(filtered_spikes, parameter_values[1]) 

def test_nest_get_spikes_parameter_fromTime_toTime_v2(): 
    parameter_values = [10, 25, 0, 0, 0]
    parameter_set_combination = [True, True, False, False, False]

    filtered_spikes = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES_V2, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_is_valid_format(filtered_spikes)

    spikes_simulation_times_are_greater_or_equal_than(filtered_spikes, parameter_values[0])
    spikes_simulation_times_are_smaller_or_equal_than(filtered_spikes, parameter_values[1]) 
    assert(filtered_spikes[JSON_VALUE_TO_FIELD_NAME.simulationId.value] == "0:1")

#Tests a nest_get_spikes request with the "nodeIds" parameter by checking if only the requested nodeIDs are returned
def test_nest_get_spikes_parameter_nodeIds():
    node_ids = [11, 101]
    parameter_values = [0, 0, node_ids, 0, 0]
    parameter_set_combination = [False, False, True, False, False]
    
    filtered_spikes = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_is_valid_format(filtered_spikes)

    spikes_nodeIds_are_subset(filtered_spikes, parameter_values[2])

def test_nest_get_spikes_parameter_nodeIds_v2():
    node_ids = [11, 101]
    parameter_values = [0, 0, node_ids, 0, 0]
    parameter_set_combination = [False, False, True, False, False]
    
    filtered_spikes = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES_V2, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_is_valid_format(filtered_spikes)

    spikes_nodeIds_are_subset(filtered_spikes, parameter_values[2])
    assert(filtered_spikes[JSON_VALUE_TO_FIELD_NAME.simulationId.value] == "0:1")

#Test a nest_get_spikes request with the "skip" parameter by checking if a request without the "skip" parameter returns the same result but offset
@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_parameter_skip():
    parameter_values = [0, 0, 0, 4, 0]
    parameter_set_combination = [False, False, False, True, False]

    filtered_spikes = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_is_valid_format(filtered_spikes)

    spikes_has_offset_in_comparison_to(URL_NEST_GET_SPIKES, filtered_spikes, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination)

@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_parameter_skip_v2():
    parameter_values = [0, 0, 0, 4, 0]
    parameter_set_combination = [False, False, False, True, False]

    filtered_spikes = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES_V2, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_is_valid_format(filtered_spikes)

    spikes_has_offset_in_comparison_to(URL_NEST_GET_SPIKES_V2, filtered_spikes, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination)
    assert(filtered_spikes[JSON_VALUE_TO_FIELD_NAME.simulationId.value] == "0:1")

#Test a nest_get_spikes request with the "top" parameter by checking if the number of entries is smaller or equal than the desired amount
def test_nest_get_spikes_parameter_top():
    parameter_values = [0, 0, 0, 0, 23]
    parameter_set_combination = [False, False, False, False, True]

    filtered_spikes = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_is_valid_format(filtered_spikes)

    spikes_length_less_or_equal_to(filtered_spikes, parameter_values[4])

def test_nest_get_spikes_parameter_top_v2():
    parameter_values = [0, 0, 0, 0, 23]
    parameter_set_combination = [False, False, False, False, True]

    filtered_spikes = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES_V2, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_is_valid_format(filtered_spikes)

    spikes_length_less_or_equal_to(filtered_spikes, parameter_values[4])
    assert(filtered_spikes[JSON_VALUE_TO_FIELD_NAME.simulationId.value] == "0:1")

#Tests a nest_get_spikes request with the paramaters: "fromTime", "toTime", "nodeIds", "skip" and "top" by checking if the conditions for each of the parameters apply to the returned data
@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_all_parameters():
    paramater_values = [
        2,
        40,
        [14,3,4,7,11,101],
        2,
        20000
    ]
    
    filtered_spikes = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, paramater_values))
    spikes_is_valid_format(filtered_spikes)

    spikes_simulation_times_are_greater_or_equal_than(filtered_spikes, paramater_values[0])
    spikes_simulation_times_are_smaller_or_equal_than(filtered_spikes, paramater_values[1])
    spikes_nodeIds_are_subset(filtered_spikes, paramater_values[2])
    spikes_has_offset_in_comparison_to(URL_NEST_GET_SPIKES, filtered_spikes, NEST_GET_SPIKES_PARAMETER_NAME_LIST, paramater_values)
    spikes_length_less_or_equal_to(filtered_spikes, paramater_values[4])

@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_all_parameters_v2():
    paramater_values = [
        2,
        40,
        [14,3,4,7,11,101],
        2,
        20000
    ]
    
    filtered_spikes = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, paramater_values))
    spikes_is_valid_format(filtered_spikes)

    spikes_simulation_times_are_greater_or_equal_than(filtered_spikes, paramater_values[0])
    spikes_simulation_times_are_smaller_or_equal_than(filtered_spikes, paramater_values[1])
    spikes_nodeIds_are_subset(filtered_spikes, paramater_values[2])
    spikes_has_offset_in_comparison_to(URL_NEST_GET_SPIKES, filtered_spikes, NEST_GET_SPIKES_PARAMETER_NAME_LIST, paramater_values)
    spikes_length_less_or_equal_to(filtered_spikes, paramater_values[4])

#Tests every possible combination of the nest_get_spikes query-parameters "fromTime", "toTime", "nodeIds", "skip" and "top" by checking if the conditions for each of the parameters apply to the returned data
@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_parameter_combinations():
    paramater_values = [
        2,
        40,
        [14,3,4,7,11,101],
        2,
        200
    ]

    check_all_parameter_combinations(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, paramater_values)

@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_parameter_combinations_v2():
    paramater_values = [
        2,
        40,
        [14,3,4,7,11,101],
        2,
        200
    ]

    check_all_parameter_combinations(URL_NEST_GET_SPIKES_V2, NEST_GET_SPIKES_PARAMETER_NAME_LIST, paramater_values)

@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_last_frame_sim_finished():
    end_time = return_json_body_if_status_ok(BASE_REQUEST_URL + '/simulationTimeInfo')['end']
     
    spikes_without_last = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES,[NEST_GET_SPIKES_PARAMETER_NAME_LIST.toTime],[end_time - 1.0]))
    assert(spikes_without_last['lastFrame'] == 0)

    spikes_without_last = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES))
    assert(spikes_without_last['lastFrame'] == 1)

@pytest.mark.order(after="test_order.py::test_sim_finished")
def test_nest_get_spikes_last_frame_sim_finished_v2():
    end_time = return_json_body_if_status_ok(BASE_REQUEST_URL_V2 + '/simulationTimeInfo')['end']
     
    spikes_without_last = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES_V2,[NEST_GET_SPIKES_PARAMETER_NAME_LIST.toTime],[end_time - 1.0]))
    assert(spikes_without_last['lastFrame'] == 0)

    spikes_without_last = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES_V2))
    assert(spikes_without_last['lastFrame'] == 1)

@pytest.mark.order("first")
def test_nest_get_spikes_last_frame_while_running(nest_simulation,printer):
    simulation_time_info = return_json_body_if_status_ok(BASE_REQUEST_URL + '/simulationTimeInfo')
    end_time = simulation_time_info['end']
    curr_time = simulation_time_info['current']
    if curr_time == end_time:
        return
    
    spikes_without_last = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES))
    assert(spikes_without_last['lastFrame'] == False)

@pytest.mark.order("first")
def test_nest_get_spikes_last_frame_while_running_v2(nest_simulation,printer):
    simulation_time_info = return_json_body_if_status_ok(BASE_REQUEST_URL_V2 + '/simulationTimeInfo')
    end_time = simulation_time_info['end']
    curr_time = simulation_time_info['current']
    if curr_time == end_time:
        return
    
    spikes_without_last = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_SPIKES_V2))
    assert(spikes_without_last['lastFrame'] == False)
