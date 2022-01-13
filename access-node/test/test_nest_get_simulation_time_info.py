
import requests
import pytest
import time
import numbers
import math
from nest_general_test_functions import *

class NestSimulationTime:
    def __init__(self):
        self.current_time = 0.0
        self.ticked = False
@pytest.fixture(scope="session")
def simulation_time(request):
    sim_time = NestSimulationTime()
    yield sim_time
    if request.config.option.count > 1:
        assert(sim_time.ticked)

@pytest.mark.repeat(1)
def test_simulation_time_info_fields(nest_simulation, simulation_time):
     
    simulation_time_info = return_json_body_if_status_ok(BASE_REQUEST_URL + '/simulationTimeInfo')

    assert('stepSize' in simulation_time_info)
    assert(isinstance(simulation_time_info['stepSize'], numbers.Number))
    assert('current' in simulation_time_info)
    assert(isinstance(simulation_time_info['current'], numbers.Number))
    assert('begin' in simulation_time_info)
    assert(isinstance(simulation_time_info['begin'], numbers.Number))
    assert('end' in simulation_time_info)
    assert(isinstance(simulation_time_info['end'], numbers.Number))
    
    assert(simulation_time_info['begin'] >= 0)
    assert(simulation_time_info['end'] >= 0)
    assert(simulation_time_info['current'] >= 0)
    assert(simulation_time_info['begin'] <= simulation_time_info['current'])
    assert(simulation_time_info['end'] >= simulation_time_info['current'])

def test_simulation_time_info_time(nest_simulation, simulation_time):
    simulation_time_info = return_json_body_if_status_ok(BASE_REQUEST_URL + '/simulationTimeInfo')
    new_time = simulation_time_info['current']
    assert(simulation_time.current_time <= new_time)

    if(simulation_time.current_time != new_time):
        simulation_time.ticked=True
    simulation_time.current_time = new_time
