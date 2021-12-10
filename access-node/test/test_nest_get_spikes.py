import time
import numbers
import math
import itertools
from operator import truediv
import requests
from requests.api import request
from requests.sessions import Request
from nest_general_test_functions import *

#URL used for the "nest_get_spikes" HTTP-query
URL_NEST_GET_SPIKES = BASE_REQUEST_URL + "/spikes"

#Names for the "nest_get_spikes" request-parameters
class NEST_GET_SPIKES_PARAMETER_NAME_LIST (Enum):
    fromTime = "fromTime"
    toTime = "toTime"
    nodeIds = "nodeIds"
    skip = "skip"
    top = "top"

#Tests a default nest_get_spikes request withour any parameters
def test_nest_get_spikes_no_parameters():
    spikes_check_if_valid_format(check_request_valid(URL_NEST_GET_SPIKES))

#Tests a nest_get_spikes request with the "fromTime" parameter by checking if all returned times are greater or equal than requested
def test_nest_get_spikes_parameter_fromTime(): 
    parameter_values = [15, 0, 0, 0, 0]
    parameter_set_combination = [True, False, False, False, False]

    filtered_spikes = check_request_valid(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_check_if_valid_format(filtered_spikes)

    spikes_check_if_simulation_times_are_greater_or_equal_than(filtered_spikes, parameter_values[0])

#Tests a nest_get_spikes request with the "toTime" parameter by checking if all returned times are smaller or equal than requested
def test_nest_get_spikes_parameter_toTime(): 
    parameter_values = [0, 25, 0, 0, 0]
    parameter_set_combination = [False, True, False, False, False]

    filtered_spikes = check_request_valid(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_check_if_valid_format(filtered_spikes)

    spikes_check_if_simulation_times_are_smaller_or_equal_than(filtered_spikes, parameter_values[1])

#Tests a nest_get_spikes request with the "fromTime" and the "toTime" parameter by checking if all returned times are in thr requested timespan
def test_nest_get_spikes_parameter_fromTime_toTime(): 
    parameter_values = [10, 25, 0, 0, 0]
    parameter_set_combination = [True, True, False, False, False]

    filtered_spikes = check_request_valid(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_check_if_valid_format(filtered_spikes)

    spikes_check_if_simulation_times_are_greater_or_equal_than(filtered_spikes, parameter_values[0])
    spikes_check_if_simulation_times_are_smaller_or_equal_than(filtered_spikes, parameter_values[1]) 

#Tests a nest_get_spikes request with the "nodeIds" parameter by checking if only the requested nodeIDs are returned
def test_nest_get_spikes_parameter_nodeIds():
    node_ids = [11, 101]
    parameter_values = [0, 0, node_ids, 0, 0]
    parameter_set_combination = [False, False, True, False, False]
    
    filtered_spikes = check_request_valid(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_check_if_valid_format(filtered_spikes)

    spikes_check_if_only_nodeIds_included(filtered_spikes, parameter_values[2])

#Test a nest_get_spikes request with the "skip" parameter by checking if a request without the "skip" parameter returns the same result but offset
def test_nest_get_spikes_parameter_skip():
    parameter_values = [0, 0, 0, 4, 0]
    parameter_set_combination = [False, False, False, True, False]

    filtered_spikes = check_request_valid(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_check_if_valid_format(filtered_spikes)

    spikes_check_if_has_offset_to_request_with_parameters(URL_NEST_GET_SPIKES, filtered_spikes, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination)

#Test a nest_get_spikes request with the "top" parameter by checking if the number of entries is smaller or equal than the desired amount
def test_nest_get_spikes_parameter_top():
    parameter_values = [0, 0, 0, 0, 23]
    parameter_set_combination = [False, False, False, False, True]

    filtered_spikes = check_request_valid(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_check_if_valid_format(filtered_spikes)

    spikes_check_if_number_of_entries_less_or_equal_to(filtered_spikes, parameter_values[4])

#Tests a nest_get_spikes request with the paramaters: "fromTime", "toTime", "nodeIds", "skip" and "top" by checking if the conditions for each of the parameters apply to the returned data
def test_nest_get_spikes_all_parameters():
    paramater_values = [
        2,
        40,
        [14,3,4,7,11,101],
        2,
        20000
    ]
    
    filtered_spikes = check_request_valid(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, paramater_values))
    spikes_check_if_valid_format(filtered_spikes)

    spikes_check_if_simulation_times_are_greater_or_equal_than(filtered_spikes, paramater_values[0])
    spikes_check_if_simulation_times_are_smaller_or_equal_than(filtered_spikes, paramater_values[1])
    spikes_check_if_only_nodeIds_included(filtered_spikes, paramater_values[2])
    spikes_check_if_has_offset_to_request_with_parameters(URL_NEST_GET_SPIKES, filtered_spikes, NEST_GET_SPIKES_PARAMETER_NAME_LIST, paramater_values)
    spikes_check_if_number_of_entries_less_or_equal_to(filtered_spikes, paramater_values[4])

#Tests every possible combination of the nest_get_spikes query-parameters "fromTime", "toTime", "nodeIds", "skip" and "top" by checking if the conditions for each of the parameters apply to the returned data
def test_nest_get_spikes_parameter_combinations():
    paramater_values = [
        2,
        40,
        [14,3,4,7,11,101],
        2,
        200
    ]

    check_all_parameter_combinations_with_conditions_and_values(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, paramater_values)