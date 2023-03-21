import pytest
import time
from nest_general_test_functions import *  

@pytest.mark.repeat(1)
@pytest.mark.order("last")
def test_sim_finished(printer):
    printer("Waiting for sim to finish")
    while True:
        simulation_time_info = return_json_body_if_status_ok(BASE_REQUEST_URL + '/simulationTimeInfo')
        if simulation_time_info['current']  == simulation_time_info['end']:
            printer("Sim finished")
            break

    
