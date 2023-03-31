from arbor_general_test_functions import *
import pytest

#URL used for the "nest_get_spikes" HTTP-query
URL_NEST_GET_CELL_INFOS = BASE_REQUEST_URL + "/cell_infos/"
#TODO: Fix inconsistency between plural and singular regarding "cell_info(s)"

#Names for the "nest_get_spikes" request-parameters
class NEST_GET_PROBE_DATA_PARAMETER_NAME_LIST (Enum):
    gId = "gId"

def cell_infos_is_valid_format(cell_object):
    assert(JSON_VALUE_TO_FIELD_NAME.cellInfos.value in cell_object)
    cell_infos = cell_object[JSON_VALUE_TO_FIELD_NAME.cellInfos.value]
    
    assert(len(cell_infos) != 0)
    for cell_info in cell_infos:
        assert(JSON_VALUE_TO_FIELD_NAME.gid.value in cell_info)
        assert(isinstance(cell_info[JSON_VALUE_TO_FIELD_NAME.gid.value], int))

        assert(JSON_VALUE_TO_FIELD_NAME.cellKind.value in cell_info)
        assert(isinstance(cell_info[JSON_VALUE_TO_FIELD_NAME.cellKind.value], str))

        assert(JSON_VALUE_TO_FIELD_NAME.numBranches.value in cell_info)
        assert(isinstance(cell_info[JSON_VALUE_TO_FIELD_NAME.numBranches.value], int))

        assert(JSON_VALUE_TO_FIELD_NAME.numSegments.value in cell_info)
        assert(isinstance(cell_info[JSON_VALUE_TO_FIELD_NAME.numSegments.value], int))

#Tests a default nest_get_spikes request without any parameters
def test_nest_get_cell_info_no_parameters():
    cell_infos_is_valid_format(return_json_body_if_status_ok(URL_NEST_GET_CELL_INFOS))
