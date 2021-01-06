import requests
import numbers

def test_get_spikedetectors(nest_simulation):
    request = requests.get("http://localhost:8000/spikedetectors")
    assert(request.status_code == 200)
    
    spike_detectors = request.json()

    assert(isinstance(spike_detectors, list))

    for spike_detector in spike_detectors:
        assert('spikedetectorId' in spike_detector)
        assert(isinstance(spike_detector['spikedetectorId'], int))

        assert('nodeIds' in spike_detector)
        assert(isinstance(spike_detector['nodeIds'], list))

        for node_id in spike_detector['nodeIds']:
            assert(isinstance(node_id, int))
