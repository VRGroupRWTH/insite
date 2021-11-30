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
URL_NEST_GET_SPIKES = "http://localhost:8080/nest/spikes"

#Names for the "nest_get_spikes" parameters
NEST_GET_SPIKES_PARAMETER_NAME_LIST = [
    "fromTime",
    "toTime",
    "nodeIds",
    "skip",
    "top"
]

#Tests a default nest_get_spikes request withour any parameters
def test_nest_get_spikes_no_parameters():
    spikes_check_if_valid_format(nest_check_request_valid(URL_NEST_GET_SPIKES))

#Tests a nest_get_spikes request with the "fromTime" parameter
def test_nest_get_spikes_parameter_fromTime(): 
    parameter_values = [15, 0, 0, 0, 0]
    parameter_set_combination = [True, False, False, False, False]

    filtered_spikes = nest_check_request_valid(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_check_if_valid_format(filtered_spikes)

    spikes_check_if_simulation_times_have_minimum(filtered_spikes, parameter_values[0])

#Tests a nest_get_spikes request with the "toTime" parameter
def test_nest_get_spikes_parameter_toTime(): 
    parameter_values = [0, 25, 0, 0, 0]
    parameter_set_combination = [False, True, False, False, False]

    filtered_spikes = nest_check_request_valid(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_check_if_valid_format(filtered_spikes)

    spikes_check_if_simulation_times_have_maximum(filtered_spikes, parameter_values[1])

#Tests a nest_get_spikes request with the "fromTime" and the "toTime" parameter
def test_nest_get_spikes_parameter_fromTime_toTime(): 
    parameter_values = [10, 25, 0, 0, 0]
    parameter_set_combination = [True, True, False, False, False]

    filtered_spikes = nest_check_request_valid(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_check_if_valid_format(filtered_spikes)

    spikes_check_if_simulation_times_have_minimum(filtered_spikes, parameter_values[0])
    spikes_check_if_simulation_times_have_maximum(filtered_spikes, parameter_values[1]) 

#Tests a nest_get_spikes request with the "nodeIds" parameter
def test_nest_get_spikes_parameter_nodeIds():
    node_ids = [11, 101]
    parameter_values = [0, 0, node_ids, 0, 0]
    parameter_set_combination = [False, False, True, False, False]
    
    filtered_spikes = nest_check_request_valid(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_check_if_valid_format(filtered_spikes)

    spikes_check_if_only_nodeIds_included(filtered_spikes, parameter_values[2])

#Test a nest_get_spikes request with the "skip" parameter
def test_nest_get_spikes_parameter_skip():
    parameter_values = [0, 0, 0, 4, 0]
    parameter_set_combination = [False, False, False, True, False]

    filtered_spikes = nest_check_request_valid(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_check_if_valid_format(filtered_spikes)

    spikes_check_if_has_offset_with_parameters(URL_NEST_GET_SPIKES, filtered_spikes, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination)

#Test a nest_get_spikes request with the "top" parameter
def test_nest_get_spikes_parameter_top():
    parameter_values = [0, 0, 0, 0, 23]
    parameter_set_combination = [False, False, False, False, True]

    filtered_spikes = nest_check_request_valid(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    spikes_check_if_valid_format(filtered_spikes)

    spikes_check_if_number_less_or_equal_to(filtered_spikes, parameter_values[4])

#Tests a nest_get_spikes request qith all possible parameters
def test_nest_get_spikes_all_parameters():
    paramater_values = [
        2,
        40,
        [14,3,4,7,11,101],
        2,
        20000
    ]
    
    filtered_spikes = nest_check_request_valid(build_query_string(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, paramater_values))
    spikes_check_if_valid_format(filtered_spikes)

    spikes_check_if_simulation_times_have_minimum(filtered_spikes, paramater_values[0])
    spikes_check_if_simulation_times_have_maximum(filtered_spikes, paramater_values[1])
    spikes_check_if_only_nodeIds_included(filtered_spikes, paramater_values[2])
    spikes_check_if_has_offset_with_parameters(URL_NEST_GET_SPIKES, filtered_spikes, NEST_GET_SPIKES_PARAMETER_NAME_LIST, paramater_values)
    spikes_check_if_number_less_or_equal_to(filtered_spikes, paramater_values[4])

#Tests every possible combination of the nest_get_spikes query-parameters
def test_nest_get_spikes_parameter_combinations():
    paramater_values = [
        2,
        40,
        [14,3,4,7,11,101],
        2,
        200
    ]

    check_all_parameter_combinations_with_conditions_and_values(URL_NEST_GET_SPIKES, NEST_GET_SPIKES_PARAMETER_NAME_LIST, paramater_values)