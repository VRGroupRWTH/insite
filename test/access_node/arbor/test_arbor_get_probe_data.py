from arbor_general_test_functions import *
import pytest

URL_NEST_GET_PROBE_DATA = BASE_REQUEST_URL + "/probe_data/"

class NEST_GET_PROBE_DATA_PARAMETER_NAME_LIST (Enum):
    #TODO: make "x_id_" and "xId" consistent between json and params
    gId = "gId"
    lId = "lId"
    pId = "pId"
    fromTime = "fromTime"
    toTime = "toTime"

def probe_data_is_valid_format(probes_object):
    assert(JSON_VALUE_TO_FIELD_NAME.probeData.value in probes_object)
    probe_data = probes_object[JSON_VALUE_TO_FIELD_NAME.probeData.value]

    #TODO: make "probeData" and "probe_data" consistent
    
    assert(len(probe_data) != 0)
    for probe_date in probe_data:
        assert(JSON_VALUE_TO_FIELD_NAME.gid.value in probe_date)
        assert(isinstance(probe_date[JSON_VALUE_TO_FIELD_NAME.gid.value], int))

        assert(JSON_VALUE_TO_FIELD_NAME.lid.value in probe_date)
        assert(isinstance(probe_date[JSON_VALUE_TO_FIELD_NAME.lid.value], int))

        assert(JSON_VALUE_TO_FIELD_NAME.sourceIndex.value in probe_date)
        assert(isinstance(probe_date[JSON_VALUE_TO_FIELD_NAME.sourceIndex.value], int))

        assert(JSON_VALUE_TO_FIELD_NAME.size.value in probe_date)
        assert(isinstance(probe_date[JSON_VALUE_TO_FIELD_NAME.size.value], int))
        
        assert(JSON_VALUE_TO_FIELD_NAME.simulationTimes.value in probe_date)
        times = probe_date[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value]
        assert(len(times) > 0)

        assert(JSON_VALUE_TO_FIELD_NAME.gIds.value in probe_date)
        data = probe_date[JSON_VALUE_TO_FIELD_NAME.gIds.value]
        assert(len(data) > 0)

#Tests a request without any parameters
def test_nest_get_probes_no_parameters():
    probe_data_is_valid_format(return_json_body_if_status_ok(URL_NEST_GET_PROBE_DATA))

def test_nest_get_probe_data_parameter_gId(): 
    parameter_values = [2, 1, 1, 20, 80]
    parameter_set_combination = [True, False, False, False, False]

    filtered_probes_object = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_PROBE_DATA, NEST_GET_PROBE_DATA_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    probe_data_is_valid_format(filtered_probes_object)

    probe_data = filtered_probes_object[JSON_VALUE_TO_FIELD_NAME.probeData.value]
    
    assert(len(probe_data) > 0)
    for probe_date in probe_data:
        assert(probe_date[JSON_VALUE_TO_FIELD_NAME.gid.value] == parameter_values[0])

def test_nest_get_probe_data_parameter_lId(): 
    parameter_values = [2, 1, 1, 20, 80]
    parameter_set_combination = [False, True, False, False, False]

    filtered_probes_object = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_PROBE_DATA, NEST_GET_PROBE_DATA_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    probe_data_is_valid_format(filtered_probes_object)

    probe_data = filtered_probes_object[JSON_VALUE_TO_FIELD_NAME.probeData.value]
    
    assert(len(probe_data) > 0)
    for probe_date in probe_data:
        assert(probe_date[JSON_VALUE_TO_FIELD_NAME.lid.value] == parameter_values[1])

def test_nest_get_probe_data_parameter_pId(): 
    parameter_values = [2, 1, 1, 20, 80]
    parameter_set_combination = [False, False, True, False, False]

    filtered_probes_object = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_PROBE_DATA, NEST_GET_PROBE_DATA_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    probe_data_is_valid_format(filtered_probes_object)

    probe_data = filtered_probes_object[JSON_VALUE_TO_FIELD_NAME.probeData.value]
    
    assert(len(probe_data) > 0)
    for probe_date in probe_data:
        assert(probe_date[JSON_VALUE_TO_FIELD_NAME.sourceIndex.value] == parameter_values[2])

def test_nest_get_probe_data_parameter_fromTime(): 
    parameter_values = [2, 1, 1, 20, 80]
    parameter_set_combination = [False, False, False, True, False]

    filtered_probes_object = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_PROBE_DATA, NEST_GET_PROBE_DATA_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    probe_data_is_valid_format(filtered_probes_object)

    probe_data = filtered_probes_object[JSON_VALUE_TO_FIELD_NAME.probeData.value]
    
    assert(len(probe_data) > 0)
    for probe_date in probe_data:
        simTimes = probe_date[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value]
        for t in simTimes:
            assert(t >= parameter_values[3])

def test_nest_get_probe_data_parameter_toTime(): 
    parameter_values = [2, 1, 1, 20, 80]
    parameter_set_combination = [False, False, False, False, True]

    filtered_probes_object = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_PROBE_DATA, NEST_GET_PROBE_DATA_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    probe_data_is_valid_format(filtered_probes_object)

    probe_data = filtered_probes_object[JSON_VALUE_TO_FIELD_NAME.probeData.value]
    
    assert(len(probe_data) > 0)
    for probe_date in probe_data:
        simTimes = probe_date[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value]
        for t in simTimes:
            assert(t <= parameter_values[4])

def test_nest_get_probe_data_all_parameters(): 
    parameter_values = [2, 1, 0, 20, 80]
    parameter_set_combination = [True, True, True, True, True]

    filtered_probes_object = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_PROBE_DATA, NEST_GET_PROBE_DATA_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    probe_data_is_valid_format(filtered_probes_object)

    probe_data = filtered_probes_object[JSON_VALUE_TO_FIELD_NAME.probeData.value]
    
    assert(len(probe_data) > 0)

    for probe_date in probe_data:
        assert(probe_date[JSON_VALUE_TO_FIELD_NAME.gid.value] == parameter_values[0])

    for probe_date in probe_data:
        assert(probe_date[JSON_VALUE_TO_FIELD_NAME.lid.value] == parameter_values[1])

    for probe_date in probe_data:
        simTimes = probe_date[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value]
        for t in simTimes:
            assert(t >= parameter_values[3])

    for probe_date in probe_data:
        assert(probe_date[JSON_VALUE_TO_FIELD_NAME.sourceIndex.value] == parameter_values[2])

    for probe_date in probe_data:
        simTimes = probe_date[JSON_VALUE_TO_FIELD_NAME.simulationTimes.value]
        for t in simTimes:
            assert(t <= parameter_values[4])
