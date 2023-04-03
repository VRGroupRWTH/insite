import tvb_general_test_functions
from tvb_config import *

def data_has_all_fields(data):
    assert("uid" in data), f"Data member uid missing"
    assert("timesteps" in data), f"Data member timesteps missing"
    
def data_populated_all_fields(data):
    for member in data:
        assert(data[member] is not None), f"Data member {member} null"
    assert(len(data["timesteps"]) > 0), f"Data member timesteps empty"
    
def timesteps_has_all_fields(data):
    for timestep in data["timesteps"]:
        assert("time" in timestep), f"Data[timesteps] member time missing"

def timesteps_populated_all_fields(data):
    for timestep in data["timesteps"]:
        for member in timestep:
            assert(timestep[member] is not None), f"Data[timesteps] member {member} null"

def timesteps_valid(data):
    history = []
    for timestep in data["timesteps"]:
        for prev_time in history:
            assert(timestep["time"] > prev_time), f"Data[timesteps] members values not successive in time"
        history.append(timestep["time"])

def data_has_unique_ids(data_points):
    uids = []
    for data in data_points:
        assert(not data["uid"] in uids), f"Data member uid not unique"
        uids.append(data["uid"])
            
def test_monitors_exist(tvb_simulation):
    # TESTS
    datapoints = tvb_general_test_functions.return_json_body_if_status_ok(BASE_REQUEST_URL + "/monitorData")
    assert(datapoints is not None)
    assert(len(datapoints) > 0), f"Datapoints empty"
    for data in datapoints:
        data_has_all_fields(data)
        data_populated_all_fields(data)
        timesteps_has_all_fields(data)
        timesteps_populated_all_fields(data)
        timesteps_valid(data)
    data_has_unique_ids(datapoints)
