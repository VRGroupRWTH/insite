import time
import numbers
import math
import itertools
from operator import truediv
import requests
from requests.api import request
from requests.sessions import Request

#Server response for a succesfull operation
HTTP_QUERY_SUCCESS_CODE = 200

#Name that is used to access simulation times
SIMULATION_TIMES_NAME = "simulationTimes"

#Name that is used to access node IDs
NODE_IDS_NAME = "nodeIds"

#Checks if HTTP-request is valid, and returns spike-data in Json format.
def nest_check_request_valid(request):
    request = requests.get(request)
    assert(request.status_code == HTTP_QUERY_SUCCESS_CODE)
    spikes = request.json()
    return spikes

#Checks if the given spike data are in a valid format
def spikes_check_if_valid_format(spikes):
    spikes_check_data_length_valid(spikes)
    spikes_sorted_by_time(spikes)
    spikes_check_if_nodeIds_have_minimum(spikes, 0)
    spikes_check_if_simulation_times_have_minimum(spikes, 0)

#Checks if the given spike-data is sorted correctly by time
def spikes_sorted_by_time(spikes):
    previous_time = 0
    for time in spikes[SIMULATION_TIMES_NAME]:
        assert(time >= previous_time)
        previous_time = time

#Checks if nodeIds of the given spike data have the desired minimum
def spikes_check_if_nodeIds_have_minimum(spikes, minimum):
    for time in spikes[NODE_IDS_NAME]:
        assert(time >= minimum)

#Checks if the length of the given spike-data is valid
def spikes_check_data_length_valid(spikes):
    assert(NODE_IDS_NAME in spikes)
    assert(SIMULATION_TIMES_NAME in spikes)
    assert(len(spikes.keys()) == 2)
    assert(len(spikes[NODE_IDS_NAME]) == len(spikes[SIMULATION_TIMES_NAME]))
    assert(len(spikes[NODE_IDS_NAME]) > 0)

#Checks and returns whether the two given spike data is equal
def spikes_check_if_equal_data(spike_data_a, spike_data_b):
    assert(len(spike_data_a[SIMULATION_TIMES_NAME]) == len(spike_data_b[SIMULATION_TIMES_NAME]))
    assert(len(spike_data_a[NODE_IDS_NAME]) == len(spike_data_b[NODE_IDS_NAME]))
    
    for i in range(len(spike_data_a[SIMULATION_TIMES_NAME])):
        assert(spike_data_a[SIMULATION_TIMES_NAME][i] == spike_data_b[SIMULATION_TIMES_NAME][i])
        assert(spike_data_a[NODE_IDS_NAME][i] == spike_data_b[NODE_IDS_NAME][i])

#Checks if the given simulation times are all greater than the desired time
def spikes_check_if_simulation_times_have_minimum(spikes, minimum_time):
    for time in spikes[SIMULATION_TIMES_NAME]:
        assert(time >= minimum_time)
    
#Checks if the given simulation times are all smaller than the desired time
def spikes_check_if_simulation_times_have_maximum(spikes, maximum_time):
    for time in spikes[SIMULATION_TIMES_NAME]:
        assert(time <= maximum_time)

#Checks if the given spike-data only includes the desired nodeIDs
def spikes_check_if_only_nodeIds_included(spikes, nodeId_list):
    for id in spikes[NODE_IDS_NAME]:
        assert(id in nodeId_list)

#Checks if the given skipped spikes have the desired offset to the given unskipped spikes
def spikes_check_if_has_offset(skipped_spikes, non_skipped_spikes, skip):
    assert(len(skipped_spikes[NODE_IDS_NAME]) + skip == len(non_skipped_spikes[NODE_IDS_NAME]))
    assert(len(skipped_spikes[SIMULATION_TIMES_NAME]) + skip == len(non_skipped_spikes[SIMULATION_TIMES_NAME]))

    for i in range(len(skipped_spikes)):
        assert(non_skipped_spikes[NODE_IDS_NAME][skip + i] == skipped_spikes[NODE_IDS_NAME][i])
        assert(non_skipped_spikes[SIMULATION_TIMES_NAME][skip + i] == skipped_spikes[SIMULATION_TIMES_NAME][i])    

#Checks if the given spike-data has the offset defined in parameters in comparison to the spike-data with the given parameters but without the offset
def spikes_check_if_has_offset_with_parameters(REQUEST_URL, skipped_spikes, PARAMETER_NAME_LIST, nest_get_spikes_query_parameters, parameter_set_combination = [True, True, True, True, True]):
    parameter_set_combination = list(parameter_set_combination)

    if parameter_set_combination[4]:
        parameter_set_combination[3] = False
        parameter_set_combination[4] = False
    
        query_string_no_skip_no_top = build_query_string(REQUEST_URL, PARAMETER_NAME_LIST, nest_get_spikes_query_parameters, parameter_set_combination)
        spikes_no_skip_no_top = nest_check_request_valid(query_string_no_skip_no_top)
        spikes_check_if_valid_format(spikes_no_skip_no_top)
        
        startIndex = nest_get_spikes_query_parameters[3]
        endIndex = nest_get_spikes_query_parameters[3] + nest_get_spikes_query_parameters[4]

        spikes_no_skip_no_top[NODE_IDS_NAME] = spikes_no_skip_no_top[NODE_IDS_NAME][startIndex:endIndex]
        spikes_no_skip_no_top[SIMULATION_TIMES_NAME] = spikes_no_skip_no_top[SIMULATION_TIMES_NAME][startIndex:endIndex]

        spikes_check_if_has_offset(skipped_spikes, spikes_no_skip_no_top, 0)
    else:
        parameter_set_combination[3] = False
        query_string_no_skip = build_query_string(REQUEST_URL, PARAMETER_NAME_LIST, nest_get_spikes_query_parameters, parameter_set_combination)
        spikes_no_skip = nest_check_request_valid(query_string_no_skip)
        spikes_check_if_valid_format(spikes_no_skip)

        spikes_check_if_has_offset(skipped_spikes, spikes_no_skip, nest_get_spikes_query_parameters[3])

#Checks if the given spike-data the desired amount of entries
def spikes_check_if_number_less_or_equal_to(spikes, maximum_amount):
    assert(len(spikes[NODE_IDS_NAME]) <= maximum_amount)
    assert(len(spikes[SIMULATION_TIMES_NAME]) <= maximum_amount)

#Filters and returns the given spike data by deleting every entrie with a simulation time higher than the minimum time
def filter_spike_data_with_minimum_time(spikes, minimum_time):
    filtered_times = []
    filtered_nodes = []

    for i in range(len(spikes[SIMULATION_TIMES_NAME])):
        if spikes[SIMULATION_TIMES_NAME][i] >= minimum_time:
            filtered_times.append(spikes[SIMULATION_TIMES_NAME][i])
            filtered_nodes.append(spikes[NODE_IDS_NAME][i])

    spikes[SIMULATION_TIMES_NAME] = filtered_times
    spikes[NODE_IDS_NAME] = filtered_nodes

    return spikes

#Filters and returns the given spike data by deleting every entry with a simulation time lower than the maximum time
def filter_spike_data_with_maximum_time(spikes, maximum_time):
    filtered_times = []
    filtered_nodes = []

    for i in range(len(spikes[SIMULATION_TIMES_NAME])):
        if spikes[SIMULATION_TIMES_NAME][i] <= maximum_time:
            filtered_times.append(spikes[SIMULATION_TIMES_NAME][i])
            filtered_nodes.append(spikes[NODE_IDS_NAME][i])

    spikes[SIMULATION_TIMES_NAME] = filtered_times
    spikes[NODE_IDS_NAME] = filtered_nodes

    return spikes

#Filters and returns the given spike data by deleting every entry that does not belong to one of the given nodeIds
def filter_spike_data_with_nodeIds(spikes, nodeIds):
    filtered_times = []
    filtered_nodes = []

    for i in range(len(spikes[SIMULATION_TIMES_NAME])):
        if spikes[NODE_IDS_NAME][i] in nodeIds:
            filtered_times.append(spikes[SIMULATION_TIMES_NAME][i])
            filtered_nodes.append(spikes[NODE_IDS_NAME][i])

    spikes[SIMULATION_TIMES_NAME] = filtered_times
    spikes[NODE_IDS_NAME] = filtered_nodes

    return spikes

#Offsets the given spike data by the desired amount
def filter_spike_data_with_offset(spikes, offset):
    filtered_times = []
    filtered_nodes = []

    for i in range(len(spikes[SIMULATION_TIMES_NAME])):
        if i >= offset:
            filtered_times.append(spikes[SIMULATION_TIMES_NAME][i])
            filtered_nodes.append(spikes[NODE_IDS_NAME][i])

    spikes[SIMULATION_TIMES_NAME] = filtered_times
    spikes[NODE_IDS_NAME] = filtered_nodes

    return spikes

#Filters and returns the given spike data by only keeping the desired number of entries
def filter_spike_data_with_number(spikes, max_number):
    filtered_times = []
    filtered_nodes = []

    for i in range(len(spikes[SIMULATION_TIMES_NAME])):
        if i < max_number:
            filtered_times.append(spikes[SIMULATION_TIMES_NAME][i])
            filtered_nodes.append(spikes[NODE_IDS_NAME][i])

    spikes[SIMULATION_TIMES_NAME] = filtered_times
    spikes[NODE_IDS_NAME] = filtered_nodes

    return spikes

#Applies the given parameters to the given, non filtered spike data and returns the filtered result
def apply_nest_get_spikes_filters(spikes, parameter_values, parameter_set_list):
    if parameter_set_list[0]:
        spikes = filter_spike_data_with_minimum_time(spikes, parameter_values[0])
    if parameter_set_list[1]:
        spikes = filter_spike_data_with_maximum_time(spikes, parameter_values[1])
    if parameter_set_list[2]:
        spikes = filter_spike_data_with_nodeIds(spikes, parameter_values[2])
    if parameter_set_list[3]:
        spikes = filter_spike_data_with_offset(spikes, parameter_values[3])
    if parameter_set_list[4]:
        spikes = filter_spike_data_with_number(spikes, parameter_values[4])

    return spikes

#Returns a list that includes every possible combination of boolean values with a given length
def get_all_boolean_combinations(number):
    return list(itertools.product([True,False], repeat = number))

#Builds and returns a HTTP-Query String including the given parameters and values
def build_query_string(prefix_string, paramater_name_list, param_value_list, paramater_is_set_list = [True, True, True, True, True]):
    query_string = prefix_string + "?"
    is_first_paramater = True

    for i in range(len(paramater_is_set_list)):
        if paramater_is_set_list[i]:
            if is_first_paramater:
                query_string += paramater_name_list[i]
                is_first_paramater = False
            else:
                query_string += "&" + paramater_name_list[i]

            query_string += "="

            if (isinstance(param_value_list[i], list)):
                is_first_array_element = True
                
                for value in param_value_list[i]:
                    if is_first_array_element:
                        is_first_array_element = False
                        query_string += str(value)
                    else:
                        query_string += "," + str(value)
            else:
                query_string += str(param_value_list[i])
                
    return query_string

#Tests every possible combination of query-parameters with a given query prefix and the given parameters by using the given handling functions
def check_all_parameter_combinations_with_conditions_and_values(REQUEST_URL, parameter_name_list, parameter_values):
    combinations = get_all_boolean_combinations(len(parameter_name_list))

    for parameter_set_combination in combinations:
        query_string = build_query_string(REQUEST_URL, parameter_name_list, parameter_values, parameter_set_combination)
        filtered_spikes = nest_check_request_valid(query_string)
        spikes_check_if_valid_format(filtered_spikes)

        unfiltered_spikes = nest_check_request_valid(REQUEST_URL)
        spikes_check_if_valid_format(unfiltered_spikes)
        self_filtered_spikes = apply_nest_get_spikes_filters(unfiltered_spikes, parameter_values, parameter_set_combination)

        spikes_check_if_equal_data(filtered_spikes, self_filtered_spikes)