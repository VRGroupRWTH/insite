import requests
import pytest
import time
import numbers
import math


def spikes_sorted_by_time(spikes):
    previous_time = 0
    for time in spikes['simulationTimes']:
        assert(time >= previous_time)
        previous_time = time

def test_nest_spikes(nest_simulation):
    r = requests.get("http://localhost:8080/nest/spikes")
    assert(r.status_code == 200)
    spikes = r.json()
    assert(len(spikes['nodeIds']) == len(spikes['simulationTimes']))

    spikes_sorted_by_time(spikes)
    
    #-----    
    from_time = 10
    to_time = 20
    r = requests.get("http://localhost:8080/nest/spikes",  params={"fromTime": from_time, "toTime": to_time})
    assert(r.status_code == 200)
    spikes = r.json()
    assert(len(spikes['nodeIds']) == len(spikes['simulationTimes']))
    
    previous_time = 0
    for time in spikes['simulationTimes']:
        assert(time >= from_time)
        assert(time <= to_time)
        assert(time >= previous_time)
        previous_time = time
        
    
    #-----
    node_ids = [11, 101]
    r = requests.get("http://localhost:8080/nest/spikes",  params={"nodeIds": node_ids})
    assert(r.status_code == 200)
    spikes = r.json()
    assert(len(spikes['nodeIds']) == len(spikes['simulationTimes']))
    
    previous_time = 0
    for i in range(len(spikes['nodeIds'])):
        assert(spikes['nodeIds'][i] in node_ids)
        assert(spikes['simulationTimes'][i] >= previous_time)
        previous_time = time


    #-----
    skip = 10
    top = 20
    r = requests.get("http://localhost:8080/nest/spikes",  params={"skip": skip, "top": top})
    assert(r.status_code == 200)
    spikes = r.json()
    assert(len(spikes['nodeIds']) == len(spikes['simulationTimes']))
    assert(len(spikes['nodeIds']) <= top)
    spikes_sorted_by_time(spikes)

    non_skipped_spikes =  r = requests.get("http://localhost:8080/nest/spikes").json()
    assert(non_skipped_spikes['nodeIds'][skip] == spikes['nodeIds'][0])
    assert(non_skipped_spikes['simulationTimes'][skip] == spikes['simulationTimes'][0])


    #-----
    skip = 10
    top = 20    
    from_time = 10
    to_time = 20
    node_ids = [11, 101]
    r = requests.get("http://localhost:8080/nest/spikes",  params={"fromTime": from_time, "toTime": to_time, "skip": skip, "top": top, "nodeIds": node_ids})#
    assert(r.status_code == 200)
    spikes = r.json()
    assert(len(spikes['nodeIds']) == len(spikes['simulationTimes']))
    assert(len(spikes['nodeIds']) <= top)
    
    previous_time = 0
    for i in range(len(spikes['nodeIds'])):
        assert(spikes['nodeIds'][i] in node_ids)
        assert(spikes['simulationTimes'][i] >= from_time)
        assert(spikes['simulationTimes'][i] <= to_time)
        assert(spikes['simulationTimes'][i] >= previous_time)
        previous_time = time
 