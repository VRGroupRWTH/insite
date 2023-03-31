from arbor_general_test_functions import *
import pytest

#URL used for the "nest_get_spikes" HTTP-query
URL_NEST_GET_CELLS = BASE_REQUEST_URL + "/cells/"

#Names for the "nest_get_spikes" request-parameters
class NEST_GET_PROBE_DATA_PARAMETER_NAME_LIST (Enum):
    gId = "gID"
    lId = "lId"
    pId = "pId"
    uId = "uId"
    hash = "hash"

def cells_is_valid_format(cell_object):
    assert(JSON_VALUE_TO_FIELD_NAME.cells.value in cell_object)
    cells = cell_object[JSON_VALUE_TO_FIELD_NAME.cells.value]
    
    assert(len(cells) != 0)
    for cell in cells:
        assert(JSON_VALUE_TO_FIELD_NAME.gid.value in cell)
        assert(isinstance(cell[JSON_VALUE_TO_FIELD_NAME.gid.value], int))

        assert(JSON_VALUE_TO_FIELD_NAME.cellKind.value in cell)
        assert(isinstance(cell[JSON_VALUE_TO_FIELD_NAME.cellKind.value], str))

        assert(JSON_VALUE_TO_FIELD_NAME.cellDescription.value in cell)
        
        description = cell[JSON_VALUE_TO_FIELD_NAME.cellDescription.value]
        assert(len(description) > 0)

        assert(JSON_VALUE_TO_FIELD_NAME.morphology.value in description)

        assert(JSON_VALUE_TO_FIELD_NAME.label.value in description)

        assert(JSON_VALUE_TO_FIELD_NAME.decor.value in description)


#Tests a default nest_get_spikes request without any parameters
def test_nest_get_cells_no_parameters():
    cells_is_valid_format(return_json_body_if_status_ok(URL_NEST_GET_CELLS))
