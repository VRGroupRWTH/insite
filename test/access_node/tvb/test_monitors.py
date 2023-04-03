import tvb_general_test_functions
from tvb_config import *

def monitors_has_all_fields(monitor):
    assert("uid" in monitor), f"Monitor member uid missing"
    assert("internalId" in monitor), f"Monitor member internalId missing"
    assert("type" in monitor), "Monitor member type missing"
    assert("observedVariables" in monitor), "Monitor member observedVariables missing"
    
def monitors_populated_all_fields(monitor):
    for member in monitor:
        assert(monitor[member] is not None), f"Monitor member {member} null"
    assert(len(monitor["observedVariables"]) > 0), f"Monitor member observedVariables empty"
    
def monitors_has_unique_ids(monitors):
    uids = []
    ids = []
    for monitor in monitors:
        assert(not monitor["internalId"] in ids), f"internalId not unique"
        ids.append(monitor["internalId"])
        assert(not monitor["uid"] in ids), f"Monitor member uid not unique"
        uids.append(monitor["uid"])

        
def monitors_has_unique_variables(monitors):
    for monitor in monitors:        
        variables = []
        for variable in monitor["observedVariables"]:
            assert(not variable in variables), f"Monitor member observedVariables has duplicate entries"
            variables.append(variable)
            
def test_monitors_exist(tvb_simulation):
    monitors = tvb_general_test_functions.return_json_body_if_status_ok(BASE_REQUEST_URL + "/monitors")
    assert(monitors is not None)
    assert(len(monitors) > 0), f"Monitors empty"
    for monitor in monitors:
        monitors_has_all_fields(monitor)
        monitors_populated_all_fields(monitor)
    monitors_has_unique_ids(monitors)
    monitors_has_unique_variables(monitors)
