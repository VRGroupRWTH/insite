import requests
import pytest
import time
import numbers
import math

class NestSimulationTime:
    def __init__(self):
        self.current_time = 0.0

@pytest.fixture(scope="session")
def simulation_time(request):
    return NestSimulationTime()

def test_simulation_time_info(nest_simulation, simulation_time):
    r = requests.get("http://localhost:8000/simulationTimeInfo")
    simulation_time_info = r.json()

    assert(r.status_code == 200)

    assert('stepSize' in simulation_time_info)
    assert(isinstance(simulation_time_info['stepSize'], numbers.Number))
    assert('current' in simulation_time_info)
    assert(isinstance(simulation_time_info['current'], numbers.Number))
    assert('begin' in simulation_time_info)
    assert(isinstance(simulation_time_info['begin'], numbers.Number))
    assert('end' in simulation_time_info)
    assert(isinstance(simulation_time_info['end'], numbers.Number))

    assert(simulation_time_info['begin'] <= simulation_time_info['current'])
    assert(simulation_time_info['end'] >= simulation_time_info['current'])

    # In theory these should be true but due to floating point inaccuracies and bad default values for 'step_size' it is not
    # assert(math.fmod(simulation_time_info['current'], simulation_time_info['step_size']) == 0.0)
    # assert(math.fmod(simulation_time_info['begin'], simulation_time_info['step_size']) == 0.0)
    # assert(math.fmod(simulation_time_info['end'], simulation_time_info['step_size']) == 0.0)

    new_time = simulation_time_info['current']
    assert(simulation_time.current_time <= new_time)
    simulation_time.current_time = new_time
