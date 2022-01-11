import json
import time
import numbers
import math
import itertools
from operator import truediv
import requests
from requests.api import request
from requests.sessions import Request
from config import *

#Checks if the given HTTP-request has a valid http-response-code. Returns the spike-data in Json format.
def return_json_body_if_status_ok(request):
    request = requests.get(request)
    assert(request.status_code == requests.codes.ok)
    return request.json()

#Converts the given spike data to pairs out of nodeId and corresponding simulation times
def zip_spikes(spikes):
    return zip(spikes[JSON_VALUE_TO_FIELD_NAME.nodeIds.value], spikes[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value])

#Checks if the given spike data has a correct length, is sorted by time and does not include values smaller than zero
def spikes_is_valid_format(spikes):
    spikes_is_data_length_valid(spikes)
    spikes_is_sorted_by_time(spikes)
    spikes_nodeIds_are_greater_or_equal_than(spikes, 0)
    spikes_simulation_times_are_greater_or_equal_than(spikes, 0)

#Checks that every entry is greater or equal than his predecessor
def spikes_is_sorted_by_time(spikes):
    previous_time = 0
    for time in spikes[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value]:
        assert(time >= previous_time)
        previous_time = time

#Checks if nodeIds of the given spike data have the desired minimum
def spikes_nodeIds_are_greater_or_equal_than(spikes, minimum):
    for id in spikes[JSON_VALUE_TO_FIELD_NAME.nodeIds.value]:
        assert(id >= minimum)

#Checks if the length of the two given spike-data sets is equal and if exactly the two wanted data-sets are included
def spikes_is_data_length_valid(spikes, canBeEmpty = False):
    assert(JSON_VALUE_TO_FIELD_NAME.simulationTimes.value in spikes)
    assert(JSON_VALUE_TO_FIELD_NAME.nodeIds.value in spikes)
    assert(len(spikes.keys()) == 2)
    assert(len(spikes[JSON_VALUE_TO_FIELD_NAME.nodeIds.value]) == len(spikes[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value]))

    if not canBeEmpty:
        assert(len(spikes[JSON_VALUE_TO_FIELD_NAME.nodeIds.value]) > 0)

#Checks and returns whether the two given spike data-sets are equal
def spikes_is_data_equal(spike_data_a, spike_data_b):
    spike_data_a[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value] == spike_data_b[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value]
    spike_data_a[JSON_VALUE_TO_FIELD_NAME.nodeIds.value] == spike_data_b[JSON_VALUE_TO_FIELD_NAME.nodeIds.value]

#Checks if the given simulation times are all greater than the desired time
def spikes_simulation_times_are_greater_or_equal_than(spikes, minimum_time):
    for time in spikes[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value]:
        assert(time >= minimum_time)
    
#Checks if the given simulation times are all smaller than the desired time
def spikes_simulation_times_are_smaller_or_equal_than(spikes, maximum_time):
    for time in spikes[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value]:
        assert(time <= maximum_time)

#Checks if the given spike-data only includes the desired nodeIDs
def spikes_nodeIds_are_subset(spikes, nodeId_list):
    for id in spikes[JSON_VALUE_TO_FIELD_NAME.nodeIds.value]:
        assert(id in nodeId_list)

#Checks if the given skipped spikes have the desired offset in comparison to the given unskipped spikes
def spikes_has_offset(skipped_spikes, non_skipped_spikes, skip):
    assert(len(skipped_spikes[JSON_VALUE_TO_FIELD_NAME.nodeIds.value]) + skip == len(non_skipped_spikes[JSON_VALUE_TO_FIELD_NAME.nodeIds.value]))
    assert(len(skipped_spikes[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value]) + skip == len(non_skipped_spikes[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value]))

    skipped_nodeIds = non_skipped_spikes[JSON_VALUE_TO_FIELD_NAME.nodeIds.value][skip::]
    skipped_simulationTimes = non_skipped_spikes[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value][skip::]

    assert(skipped_nodeIds == skipped_spikes[JSON_VALUE_TO_FIELD_NAME.nodeIds.value])
    assert(skipped_simulationTimes == skipped_spikes[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value])

#Receives an list of spikes as input that was queried with the offset set to a value > 0.
#Method queries all spikes again and applies the offset afterwards to compare the result with the list given as the input.
#Returns true if both lists are equal, false otherwise.
def spikes_has_offset_in_comparison_to(REQUEST_URL, skipped_spikes, PARAMETER_NAME_LIST, nest_get_spikes_query_parameters, parameter_set_combination = [True, True, True, True, True]):
    parameter_set_combination = list(parameter_set_combination)

    if parameter_set_combination[4]:
        parameter_set_combination[3] = False
        parameter_set_combination[4] = False
    
        query_string_no_skip_no_top = build_query_string(REQUEST_URL, PARAMETER_NAME_LIST, nest_get_spikes_query_parameters, parameter_set_combination)
        spikes_no_skip_no_top = return_json_body_if_status_ok(query_string_no_skip_no_top)
        spikes_is_valid_format(spikes_no_skip_no_top)
        
        startIndex = nest_get_spikes_query_parameters[3]
        endIndex = nest_get_spikes_query_parameters[3] + nest_get_spikes_query_parameters[4]

        spikes_no_skip_no_top[JSON_VALUE_TO_FIELD_NAME.nodeIds.value] = spikes_no_skip_no_top[JSON_VALUE_TO_FIELD_NAME.nodeIds.value][startIndex:endIndex]
        spikes_no_skip_no_top[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value] = spikes_no_skip_no_top[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value][startIndex:endIndex]

        spikes_has_offset(skipped_spikes, spikes_no_skip_no_top, 0)
    else:
        parameter_set_combination[3] = False
        query_string_no_skip = build_query_string(REQUEST_URL, PARAMETER_NAME_LIST, nest_get_spikes_query_parameters, parameter_set_combination)
        spikes_no_skip = return_json_body_if_status_ok(query_string_no_skip)
        spikes_is_valid_format(spikes_no_skip)

        spikes_has_offset(skipped_spikes, spikes_no_skip, nest_get_spikes_query_parameters[3])

#Receives a list of spikes and a maximum_number of spikes as input
#Checks if the length of the given spike-data is less or equal to the maximum_number
#Returns true if this is the case, false otherwise
def spikes_length_less_or_equal_to(spikes, maximum_number):
    assert(len(spikes[JSON_VALUE_TO_FIELD_NAME.nodeIds.value]) <= maximum_number)
    assert(len(spikes[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value]) <= maximum_number)

#Filters and returns the given spike data by deleting every entrie with a simulation time higher than the minimum time
def get_spikes_with_minimum_time(spikes, minimum_time):
    filtered_times = []
    filtered_nodes = []

    for spike_pair in zip_spikes(spikes):
        node_id, time = spike_pair
        if time >= minimum_time:
            filtered_times.append(time)
            filtered_nodes.append(node_id)

    spikes[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value] = filtered_times
    spikes[JSON_VALUE_TO_FIELD_NAME.nodeIds.value] = filtered_nodes

    return spikes

#Filters and returns the given spike data by deleting every entry with a simulation time lower than the maximum time
def get_spikes_with_maximum_time(spikes, maximum_time):
    filtered_times = []
    filtered_nodes = []

    for spike_pair in zip_spikes(spikes):
        node_id, time = spike_pair
        if time <= maximum_time:
            filtered_times.append(time)
            filtered_nodes.append(node_id)

    spikes[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value] = filtered_times
    spikes[JSON_VALUE_TO_FIELD_NAME.nodeIds.value] = filtered_nodes

    return spikes

#Filters and returns the given spike data by deleting every entry that does not belong to one of the given nodeIds
def get_spikes_with_nodeIds(spikes, nodeIds):
    filtered_times = []
    filtered_nodes = []

    for spike_pair in zip_spikes(spikes):
        node_id, time = spike_pair
        if node_id in nodeIds:
            filtered_times.append(time)
            filtered_nodes.append(node_id)

    spikes[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value] = filtered_times
    spikes[JSON_VALUE_TO_FIELD_NAME.nodeIds.value] = filtered_nodes

    return spikes

#Offsets the given spike data by the desired amount and returns the result
#i.e. Given spikes pairs 1..o..N returns o..N given o as offset.
def get_offset_spike_data(spikes, offset):
    filtered_times = []
    filtered_nodes = []

    count = 0
    for spike_pair in zip_spikes(spikes):
        node_id, time = spike_pair
        if count >= offset:
            filtered_times.append(time)
            filtered_nodes.append(node_id)
        count = count + 1

    spikes[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value] = filtered_times
    spikes[JSON_VALUE_TO_FIELD_NAME.nodeIds.value] = filtered_nodes

    return spikes

#Filters and returns the given spike data by only keeping the desired number of top-entries. 
#E.g. Applying the function to [6,4,2,5,0] with a "max_number" of 2 would result in [6,4]
def get_top_spike_data(spikes, max_number):
    filtered_times = []
    filtered_nodes = []

    count = 0
    for spike_pair in zip_spikes(spikes):
        node_id, time = spike_pair
        if count < max_number:
            filtered_times.append(time)
            filtered_nodes.append(node_id)
        count = count + 1

    spikes[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value] = filtered_times
    spikes[JSON_VALUE_TO_FIELD_NAME.nodeIds.value] = filtered_nodes

    return spikes

#Applies the given parameters to the given, non filtered spike data and returns the filtered result
def filter_spike_data_with_parameters(spikes, parameter_values, parameter_set_list):
    if parameter_set_list[0]:
        spikes = get_spikes_with_minimum_time(spikes, parameter_values[0])
    if parameter_set_list[1]:
        spikes = get_spikes_with_maximum_time(spikes, parameter_values[1])
    if parameter_set_list[2]:
        spikes = get_spikes_with_nodeIds(spikes, parameter_values[2])
    if parameter_set_list[3]:
        spikes = get_offset_spike_data(spikes, parameter_values[3])
    if parameter_set_list[4]:
        spikes = get_top_spike_data(spikes, parameter_values[4])

    return spikes

#Returns a list that includes every possible combination of boolean values with a given length
def get_all_boolean_combinations(number):
    return list(itertools.product([True,False], repeat = number))

#Builds and returns a HTTP-Query String including the given parameters and values
def build_query_string(prefix_string, paramater_name_list, param_value_list, paramater_is_set_list = [True, True, True, True, True]):
    query_string = prefix_string + "?"
    is_first_paramater = True

    i = 0
    for param in paramater_name_list:
        if paramater_is_set_list[i]:
            if is_first_paramater:
                query_string += param.value
                is_first_paramater = False
            else:
                query_string += "&" + param.value

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
        i = i + 1        
    return query_string

#Tests every possible combination of query-parameters with a given query prefix and the given parameters by using the given handling functions
def check_all_parameter_combinations(REQUEST_URL, parameter_name_list, parameter_values):
    combinations = get_all_boolean_combinations(len(parameter_name_list))

    for parameter_set_combination in combinations:
        query_string = build_query_string(REQUEST_URL, parameter_name_list, parameter_values, parameter_set_combination)
        filtered_spikes = return_json_body_if_status_ok(query_string)
        spikes_is_valid_format(filtered_spikes)

        unfiltered_spikes = return_json_body_if_status_ok(REQUEST_URL)
        spikes_is_valid_format(unfiltered_spikes)
        self_filtered_spikes = filter_spike_data_with_parameters(unfiltered_spikes, parameter_values, parameter_set_combination)

        spikes_is_data_equal(filtered_spikes, self_filtered_spikes)