import requests
import pytest
import time

class SimulationTime:
    def __init__(self):
        self.current_time = 0.0

@pytest.fixture(scope="session")
def simulation_time(request):
    return SimulationTime()
    
def test_current_time(nest_simulation, simulation_time):
    r = requests.get("http://localhost:8080/simulation_time_info")
    simulation_time_info = r.json()
    new_time = simulation_time_info['current']
    assert(simulation_time.current_time <= new_time)
    simulation_time.current_time = new_time

def test_sleep():
    time.sleep(1.0)
