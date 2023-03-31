from arbor_general_test_functions import *
import pytest

#URL used for the arbor HTTP-query
URL_NEST_GET_PROBES = BASE_REQUEST_URL + "/probes/"

#Names for the arbor request-parameters
class NEST_GET_PROBES_PARAMETER_NAME_LIST (Enum):
    gId = "gId"
    lId = "lId"
    pId = "pId"
    #TODO: Rename pId to source_index or unify otherwise
    uId = "uId"
    #TODO: Rename uId to probe_global_index or unify otherwise
    hash = "hash"

def probes_is_valid_format(probes_object):
    assert(JSON_VALUE_TO_FIELD_NAME.probes.value in probes_object)
    probes = probes_object[JSON_VALUE_TO_FIELD_NAME.probes.value]
    
    assert(len(probes) != 0)
    for probe in probes:
        assert(JSON_VALUE_TO_FIELD_NAME.cellGid.value in probe)
        assert(isinstance(probe[JSON_VALUE_TO_FIELD_NAME.cellGid.value], int))
        
        assert(JSON_VALUE_TO_FIELD_NAME.cellLid.value in probe)
        assert(isinstance(probe[JSON_VALUE_TO_FIELD_NAME.cellLid.value], int))

        assert(JSON_VALUE_TO_FIELD_NAME.sourceIndex.value in probe)
        assert(isinstance(probe[JSON_VALUE_TO_FIELD_NAME.sourceIndex.value], int))

        assert(JSON_VALUE_TO_FIELD_NAME.probeKind.value in probe)
        assert(isinstance(probe[JSON_VALUE_TO_FIELD_NAME.probeKind.value], str))

        assert(JSON_VALUE_TO_FIELD_NAME.hash.value in probe)
        assert(isinstance(probe[JSON_VALUE_TO_FIELD_NAME.hash.value], int))

        assert(JSON_VALUE_TO_FIELD_NAME.globalProbeIndex.value in probe)
        assert(isinstance(probe[JSON_VALUE_TO_FIELD_NAME.globalProbeIndex.value], int))

        assert(JSON_VALUE_TO_FIELD_NAME.location.value in probe)

def test_nest_get_probes_no_parameters():
    probes_is_valid_format(return_json_body_if_status_ok(URL_NEST_GET_PROBES))

def test_nest_get_probes_parameter_gId(): 
    parameter_values = [2, 1, 1, 0, 4294967298]
    parameter_set_combination = [True, False, False, False, False]

    filtered_probes_object = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_PROBES, NEST_GET_PROBES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    probes_is_valid_format(filtered_probes_object)

    probes = filtered_probes_object[JSON_VALUE_TO_FIELD_NAME.probes.value]
    
    assert(len(probes) != 0)
    for probe in probes:
        assert(probe[JSON_VALUE_TO_FIELD_NAME.cellGid.value] == parameter_values[0])

def test_nest_get_probes_parameter_lId(): 
    parameter_values = [2, 1, 1, 0, 4294967298]
    parameter_set_combination = [False, True, False, False, False]

    filtered_probes_object = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_PROBES, NEST_GET_PROBES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    probes_is_valid_format(filtered_probes_object)

    probes = filtered_probes_object[JSON_VALUE_TO_FIELD_NAME.probes.value]
    
    assert(len(probes) != 0)
    for probe in probes:
        assert(probe[JSON_VALUE_TO_FIELD_NAME.cellLid.value] == parameter_values[1])

def test_nest_get_probes_parameter_pId(): 
    parameter_values = [2, 1, 1, 0, 4294967298]
    parameter_set_combination = [False, False, True, False, False]

    filtered_probes_object = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_PROBES, NEST_GET_PROBES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    probes_is_valid_format(filtered_probes_object)

    probes = filtered_probes_object[JSON_VALUE_TO_FIELD_NAME.probes.value]
    
    assert(len(probes) != 0)
    for probe in probes:
        assert(probe[JSON_VALUE_TO_FIELD_NAME.sourceIndex.value] == parameter_values[2])

def test_nest_get_probes_parameter_uId(): 
    parameter_values = [2, 1, 1, 7, 4294967298]
    parameter_set_combination = [False, False, False, True, False]

    filtered_probes_object = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_PROBES, NEST_GET_PROBES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    probes_is_valid_format(filtered_probes_object)

    probes = filtered_probes_object[JSON_VALUE_TO_FIELD_NAME.probes.value]
    
    assert(len(probes) == 1)
    for probe in probes:
        assert(probe[JSON_VALUE_TO_FIELD_NAME.globalProbeIndex.value] == parameter_values[3])

def test_nest_get_probes_parameter_hash(): 
    parameter_values = [2, 1, 1, 7, 4294967298]
    parameter_set_combination = [False, False, False, False, True]

    filtered_probes_object = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_PROBES, NEST_GET_PROBES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    probes_is_valid_format(filtered_probes_object)

    probes = filtered_probes_object[JSON_VALUE_TO_FIELD_NAME.probes.value]
    
    assert(len(probes) == 1)
    for probe in probes:
        assert(probe[JSON_VALUE_TO_FIELD_NAME.hash.value] == parameter_values[4])

def test_nest_get_probes_all_parameters(): 
    parameter_values = [0, 1, 0, 3, 4294967296]
    parameter_set_combination = [True, True, True, True, True]

    filtered_probes_object = return_json_body_if_status_ok(build_query_string(URL_NEST_GET_PROBES, NEST_GET_PROBES_PARAMETER_NAME_LIST, parameter_values, parameter_set_combination))
    probes_is_valid_format(filtered_probes_object)

    probes = filtered_probes_object[JSON_VALUE_TO_FIELD_NAME.probes.value]
    
    assert(len(probes) == 1)
    for probe in probes:
        assert(probe[JSON_VALUE_TO_FIELD_NAME.cellGid.value] == parameter_values[0])
        assert(probe[JSON_VALUE_TO_FIELD_NAME.cellLid.value] == parameter_values[1])
        assert(probe[JSON_VALUE_TO_FIELD_NAME.sourceIndex.value] == parameter_values[2])
        assert(probe[JSON_VALUE_TO_FIELD_NAME.globalProbeIndex.value] == parameter_values[3])
        assert(probe[JSON_VALUE_TO_FIELD_NAME.hash.value] == parameter_values[4])
