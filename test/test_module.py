import requests

def test_spikes(nest_simulation):
    r = requests.get("http://localhost:8000/spikes")
    spikes = r.json()
    assert(len(spikes['gids']) == len(spikes['simulation_times']))
    
    previous_time = 0
    for time in spikes['simulation_times']:
        assert(time >= previous_time)
        previous_time = time
